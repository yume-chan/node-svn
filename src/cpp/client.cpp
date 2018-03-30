#include "client.hpp"

#include <utility>

#include <apr_hash.h>
#include <apr_pools.h>

#include <svn_client.h>
#include <svn_compat.h>
#include <svn_path.h>

#include "malloc.hpp"
#include "type_conversion.hpp"

static svn_error_t* throw_on_malfunction(svn_boolean_t can_return,
                                         const char*   file,
                                         int           line,
                                         const char*   expr) {
    throw_error(svn_error_raise_on_malfunction(true, file, line, expr));
    return nullptr;
}

struct child_pool {
    explicit child_pool(apr_pool_t* parent) {
        check_result(apr_pool_create_ex(&_pool, parent, nullptr, nullptr));
    }

    operator apr_pool_t*() {
        return _pool;
    }

    ~child_pool() {
        apr_pool_destroy(_pool);
    }

  private:
    apr_pool_t* _pool;
};

template <class T>
static auto get_reference(void* ref) {
    return static_cast<std::reference_wrapper<const T>*>(ref)->get();
}

static svn_error_t* invoke_log_message(const char**              log_msg,
                                       const char**              tmp_file,
                                       const apr_array_header_t* commit_items,
                                       void*                     raw_baton,
                                       apr_pool_t*               pool) {
    auto message = get_reference<std::string>(raw_baton);
    *log_msg     = duplicate_string(pool, message);
    return nullptr;
}

static void invoke_notify(void*                  raw_baton,
                          const svn_wc_notify_t* notify,
                          apr_pool_t*            pool) {
    auto client = static_cast<svn::client*>(raw_baton);

    svn::notify_info info{static_cast<svn::notify_action>(notify->action)};

    client->invoke_notify_function(info);
}

static svn_error_t* invoke_get_simple_prompt_provider(svn_auth_cred_simple_t** credential,
                                                      void*                    raw_baton,
                                                      const char*              raw_realm,
                                                      const char*              raw_username,
                                                      svn_boolean_t            may_save,
                                                      apr_pool_t*              pool) {
    auto client   = static_cast<svn::client*>(raw_baton);
    auto realm    = std::string(raw_realm);
    auto username = std::string(raw_username);

    auto result = client->invoke_simple_auth_providers(realm, username, may_save);
    if (result) {
        auto value      = new (pool) svn_auth_cred_simple_t;
        value->username = duplicate_string(pool, result->username);
        value->password = duplicate_string(pool, result->password);
        value->may_save = result->may_save;
        *credential     = value;
    }

    return SVN_NO_ERROR;
}

decltype(auto) read_config(const char* path, apr_pool_t* pool) {
    check_result(svn_config_ensure(path, pool));

    apr_hash_t* config;
    check_result(svn_config_get_config(&config, path, pool));

    return config;
}

static svn_error_t* cancel_func(void* raw_baton) {
    auto client = static_cast<svn::client*>(raw_baton);
    return client->invoke_abort_function() ? svn_error_create(SVN_ERR_CANCELLED, nullptr, nullptr) : nullptr;
}

namespace svn {
bool client::_apr_initialized = false;

client::client(const std::optional<const std::string>& config_path) {
    if (!_apr_initialized) {
        apr_initialize();
    }

    check_result(apr_pool_create_ex(&_pool, nullptr, nullptr, nullptr));

    auto raw_config_path = convert_from_path(config_path, _pool);
    auto config          = read_config(raw_config_path, _pool);

    check_result(svn_client_create_context2(&_context, config, _pool));

    svn_error_set_malfunction_handler(throw_on_malfunction);

    auto providers = apr_array_make(_pool, 10, sizeof(svn_auth_provider_object_t*));

    svn_auth_provider_object_t* provider;
    svn_auth_get_simple_provider2(&provider, nullptr, nullptr, _pool);
    APR_ARRAY_PUSH(providers, svn_auth_provider_object_t*) = provider;

    svn_auth_get_simple_prompt_provider(&provider, invoke_get_simple_prompt_provider, this, 0, _pool);
    APR_ARRAY_PUSH(providers, svn_auth_provider_object_t*) = provider;

    svn_auth_get_username_provider(&provider, _pool);
    APR_ARRAY_PUSH(providers, svn_auth_provider_object_t*) = provider;

    svn_auth_baton_t* auth_baton;
    svn_auth_open(&auth_baton, providers, _pool);

    const char* path;
    check_result(svn_config_get_user_config_path(&path, nullptr, nullptr, _pool));
    svn_auth_set_parameter(auth_baton, SVN_AUTH_PARAM_CONFIG_DIR, raw_config_path);

    _context->auth_baton = auth_baton;

    _context->log_msg_func3 = invoke_log_message;

    _context->notify_baton2 = this;
    _context->notify_func2  = invoke_notify;

    _context->cancel_baton = this;
    _context->cancel_func  = cancel_func;
}

client::client(client&& other)
    : _pool(std::exchange(other._pool, nullptr))
    , _context(std::exchange(other._context, nullptr)) {
}

client& client::operator=(client&& other) {
    if (this != &other) {
        if (_pool != nullptr) {
            apr_pool_destroy(_pool);
            // apr_terminate();
        }

        _pool    = std::exchange(other._pool, nullptr);
        _context = std::exchange(other._context, nullptr);
    }
    return *this;
}

client::~client() {
    if (_pool != nullptr) {
        apr_pool_destroy(_pool);
        // apr_terminate();
    }
}

void client::set_abort_function(abort_function& function) {
    _abort_function = function;
}

void client::clear_abort_functioon() {
    _abort_function = {};
}

bool client::invoke_abort_function() {
    return _abort_function ? _abort_function->operator()() : false;
}

void client::add_notify_function(std::initializer_list<notify_action> actions,
                                 const notify_function                function) {
    for (auto action : actions) {
        auto set = _notify_functions[action];
        set.insert(function);
    }
}

void client::remove_notify_function(std::initializer_list<notify_action> actions,
                                    const notify_function                function) {
    for (auto action : actions) {
        auto set = _notify_functions[action];
        set.insert(function);
    }
}

void client::invoke_notify_function(const notify_info& info) {
    for (auto function : _notify_functions[info.action]) {
        (*function)(info);
    }
}

void client::add_simple_auth_provider(const simple_auth_provider provider) {
    _simple_auth_providers.insert(provider);
}

void client::remove_simple_auth_provider(const simple_auth_provider provider) {
    _simple_auth_providers.erase(provider);
}

std::optional<simple_auth> client::invoke_simple_auth_providers(const std::string&                      realm,
                                                                const std::optional<const std::string>& username,
                                                                bool                                    may_save) {
    for (auto provider : _simple_auth_providers) {
        auto auth = (*provider)(realm, username, may_save);
        if (auth)
            return auth;
    }
    return {};
}

void client::add_to_changelist(const std::vector<std::string>&                      paths,
                               const std::string&                                   changelist,
                               svn::depth                                           depth,
                               const std::optional<const std::vector<std::string>>& changelists) const {
    child_pool pool(_pool);

    auto raw_paths       = convert_from_vector(paths, pool, true);
    auto raw_changelist  = convert_from_string(changelist);
    auto raw_changelists = convert_from_vector(changelists, pool);

    check_result(svn_client_add_to_changelist(raw_paths,
                                              raw_changelist,
                                              static_cast<svn_depth_t>(depth),
                                              raw_changelists,
                                              _context,
                                              pool));
}

static svn_error_t* invoke_get_changelists(void*       raw_baton,
                                           const char* path,
                                           const char* changelist,
                                           apr_pool_t* pool) {
    // WTF: call `get_changelists` after `remove_from_changelists`,
    // svn will return paths without any changelist.
    if (changelist == nullptr) {
        return nullptr;
    }

    auto callback = get_reference<client::get_changelists_callback>(raw_baton);
    callback(path, changelist);
    return nullptr;
}

void client::get_changelists(const std::string&                                   path,
                             const get_changelists_callback&                      callback,
                             svn::depth                                           depth,
                             const std::optional<const std::vector<std::string>>& changelists) const {
    child_pool pool(_pool);

    auto raw_path        = convert_from_path(path, pool);
    auto raw_changelists = convert_from_vector(changelists, pool);
    auto callback_ref    = std::cref(callback);

    check_result(svn_client_get_changelists(raw_path,
                                            raw_changelists,
                                            static_cast<svn_depth_t>(depth),
                                            invoke_get_changelists,
                                            &callback_ref,
                                            _context,
                                            pool));
}

void client::remove_from_changelists(const std::vector<std::string>&                      paths,
                                     svn::depth                                           depth,
                                     const std::optional<const std::vector<std::string>>& changelists) const {
    child_pool pool(_pool);

    auto raw_paths       = convert_from_vector(paths, pool, true);
    auto raw_changelists = convert_from_vector(changelists, pool);

    check_result(svn_client_remove_from_changelists(raw_paths,
                                                    static_cast<svn_depth_t>(depth),
                                                    raw_changelists,
                                                    _context,
                                                    pool));
}

void client::add(const std::string& path,
                 svn::depth         depth,
                 bool               force,
                 bool               no_ignore,
                 bool               no_autoprops,
                 bool               add_parents) const {
    child_pool pool(_pool);

    auto raw_path = convert_from_path(path, pool);

    check_result(svn_client_add5(raw_path,
                                 static_cast<svn_depth_t>(depth),
                                 force,
                                 no_ignore,
                                 no_autoprops,
                                 add_parents,
                                 _context,
                                 pool));
}

static svn_error_t* invoke_blame_callback(void*         baton,
                                          svn_revnum_t  start_revnum,
                                          svn_revnum_t  end_revnum,
                                          apr_int64_t   line_no,
                                          svn_revnum_t  revision,
                                          apr_hash_t*   rev_props,
                                          svn_revnum_t  merged_revision,
                                          apr_hash_t*   merged_rev_props,
                                          const char*   merged_path,
                                          const char*   line,
                                          svn_boolean_t local_change,
                                          apr_pool_t*   pool) {
    auto callback = get_reference<client::blame_callback>(baton);
    callback(start_revnum,
             end_revnum,
             line_no,
             convert_to_revision_number(revision),
             convert_to_revision_number(merged_revision),
             merged_path,
             line,
             local_change);
    return nullptr;
}

void client::blame(const std::string&    path,
                   const revision&       start_revision,
                   const revision&       end_revision,
                   const blame_callback& callback,
                   const revision&       peg_revision,
                   diff_ignore_space     ignore_space,
                   bool                  ignore_eol_style,
                   bool                  ignore_mime_type,
                   bool                  include_merged_revisions) const {
    child_pool pool(_pool);

    auto raw_path           = convert_from_path(path, pool);
    auto raw_start_revision = convert_from_revision(start_revision);
    auto raw_end_revision   = convert_from_revision(end_revision);
    auto raw_peg_revision   = convert_from_revision(peg_revision);
    auto callback_ref       = std::cref(callback);

    auto options              = svn_diff_file_options_create(pool);
    options->ignore_space     = static_cast<svn_diff_file_ignore_space_t>(ignore_space);
    options->ignore_eol_style = ignore_eol_style;

    check_result(svn_client_blame5(raw_path,
                                   &raw_peg_revision,
                                   &raw_start_revision,
                                   &raw_end_revision,
                                   options,
                                   ignore_mime_type,
                                   include_merged_revisions,
                                   invoke_blame_callback,
                                   &callback_ref,
                                   _context,
                                   pool));
}

svn_error_t* invoke_cat_callback(void*       raw_baton,
                                 const char* data,
                                 apr_size_t* len) {
    auto callback = get_reference<client::cat_callback>(raw_baton);
    callback(data, *len);

    return nullptr;
}

string_map client::cat(const std::string&  path,
                       const cat_callback& callback,
                       const revision&     peg_revision,
                       const revision&     revision,
                       bool                expand_keywords) const {
    child_pool pool(_pool);

    apr_hash_t* raw_properties;

    auto raw_path         = convert_from_path(path, pool);
    auto raw_callback     = std::cref(callback);
    auto raw_peg_revision = convert_from_revision(peg_revision);
    auto raw_revision     = convert_from_revision(revision);

    auto stream = svn_stream_create(&raw_callback, pool);
    svn_stream_set_write(stream, invoke_cat_callback);

    child_pool scratch_pool(_pool);
    check_result(svn_client_cat3(&raw_properties,
                                 stream,
                                 raw_path,
                                 &raw_peg_revision,
                                 &raw_revision,
                                 expand_keywords,
                                 _context,
                                 pool,
                                 scratch_pool));

    string_map result;

    apr_hash_index_t* index;
    const char*       key;
    size_t            key_size;
    svn_string_t*     value;
    for (index = apr_hash_first(_pool, raw_properties); index; index = apr_hash_next(index)) {
        apr_hash_this(index, reinterpret_cast<const void**>(&key), reinterpret_cast<apr_ssize_t*>(&key_size), reinterpret_cast<void**>(&value));

        result.emplace(std::piecewise_construct,
                       std::forward_as_tuple(key, key_size),
                       std::forward_as_tuple(value->data, value->len));
    }

    return result;
}

cat_result client::cat(const std::string& path,
                       const revision&    peg_revision,
                       const revision&    revision,
                       bool               expand_keywords) const {
    auto content  = std::vector<char>();
    auto callback = [&content](const char* data, size_t length) -> void {
        auto end = data + length;
        content.insert(content.end(), data, end);
    };

    auto properties = cat(path, callback, peg_revision, revision, expand_keywords);

    return cat_result{content, properties};
}

int32_t client::checkout(const std::string& url,
                         const std::string& path,
                         const revision&    peg_revision,
                         const revision&    revision,
                         svn::depth         depth,
                         bool               ignore_externals,
                         bool               allow_unver_obstructions) const {
    child_pool pool(_pool);

    auto raw_url          = convert_from_url(url, pool);
    auto raw_path         = convert_from_path(path, pool);
    auto raw_peg_revision = convert_from_revision(peg_revision);
    auto raw_revision     = convert_from_revision(revision);

    int32_t result_rev;

    check_result(svn_client_checkout3(reinterpret_cast<svn_revnum_t*>(&result_rev),
                                      raw_url,
                                      raw_path,
                                      &raw_peg_revision,
                                      &raw_revision,
                                      static_cast<svn_depth_t>(depth),
                                      ignore_externals,
                                      allow_unver_obstructions,
                                      _context,
                                      pool));

    return result_rev;
}

void client::cleanup(const std::string& path,
                     bool               break_locks,
                     bool               fix_recorded_timestamps,
                     bool               clear_dav_cache,
                     bool               vacuum_pristines,
                     bool               include_externals) const {
    child_pool pool(_pool);

    auto raw_path = convert_from_path(path, pool);

    check_result(svn_client_cleanup2(raw_path,
                                     break_locks,
                                     fix_recorded_timestamps,
                                     clear_dav_cache,
                                     vacuum_pristines,
                                     include_externals,
                                     _context,
                                     pool));
}

static svn_error_t* invoke_commit(const svn_commit_info_t* commit_info,
                                  void*                    raw_baton,
                                  apr_pool_t*              raw_pool) {
    auto callback = get_reference<client::commit_callback>(raw_baton);
    callback(convert_to_commit_info(commit_info));
    return nullptr;
}

void client::commit(const std::vector<std::string>&                      paths,
                    const std::string&                                   message,
                    const commit_callback&                               callback,
                    svn::depth                                           depth,
                    const std::optional<const std::vector<std::string>>& changelists,
                    const string_map&                                    revprop_table,
                    bool                                                 keep_locks,
                    bool                                                 keep_changelists,
                    bool                                                 commit_as_operations,
                    bool                                                 include_file_externals,
                    bool                                                 include_dir_externals) const {
    check_string(message);
    auto message_ref         = std::cref(message);
    _context->log_msg_baton3 = &message_ref;

    child_pool pool(_pool);

    auto raw_paths       = convert_from_vector(paths, pool, true);
    auto raw_changelists = convert_from_vector(changelists, pool);
    auto raw_props       = convert_from_map(revprop_table, _pool);
    auto callback_ref    = std::cref(callback);

    check_result(svn_client_commit6(raw_paths,
                                    static_cast<svn_depth_t>(depth),
                                    keep_locks,
                                    keep_changelists,
                                    commit_as_operations,
                                    include_file_externals,
                                    include_dir_externals,
                                    raw_changelists,
                                    raw_props,
                                    invoke_commit,
                                    &callback_ref,
                                    _context,
                                    pool));
}

static svn_error_t* invoke_info(void*                     raw_baton,
                                const char*               path,
                                const svn_client_info2_t* raw_info,
                                apr_pool_t*               raw_scratch_pool) {
    auto callback = get_reference<client::info_callback>(raw_baton);
    callback(path, convert_to_info(raw_info));
    return nullptr;
}

void client::info(const std::string&                                   path,
                  const info_callback&                                 callback,
                  const revision&                                      peg_revision,
                  const revision&                                      revision,
                  svn::depth                                           depth,
                  bool                                                 fetch_excluded,
                  bool                                                 fetch_actual_only,
                  bool                                                 include_externals,
                  const std::optional<const std::vector<std::string>>& changelists) const {
    child_pool pool(_pool);

    auto raw_path         = convert_from_path(path, pool);
    auto callback_ref     = std::cref(callback);
    auto raw_peg_revision = convert_from_revision(peg_revision);
    auto raw_revision     = convert_from_revision(revision);
    auto raw_changelists  = convert_from_vector(changelists, pool);

    check_result(svn_client_info4(raw_path,
                                  &raw_peg_revision,
                                  &raw_revision,
                                  static_cast<svn_depth_t>(depth),
                                  fetch_excluded,
                                  fetch_actual_only,
                                  include_externals,
                                  raw_changelists,
                                  invoke_info,
                                  &callback_ref,
                                  _context,
                                  pool));
}

static svn_error_t* invoke_log(void* raw_baton, svn_log_entry_t* raw_entry, apr_pool_t* pool) {
    auto entry = svn::log_entry{
        static_cast<int32_t>(raw_entry->revision),
        {},
        nullptr,
        nullptr,
        nullptr,
        static_cast<bool>(raw_entry->has_children),
        std::map<std::string, svn::log_changed_path>(),
        static_cast<bool>(raw_entry->non_inheritable),
        static_cast<bool>(raw_entry->subtractive_merge)};
    svn_compat_log_revprops_out(&entry.author, &entry.date, &entry.message, raw_entry->revprops);

    auto callback = get_reference<client::log_callback>(raw_baton);
    callback(entry);

    return nullptr;
}

void client::log(const std::vector<std::string>&                              paths,
                 const log_callback&                                          callback,
                 const std::optional<const std::vector<svn::revision_range>>& revision_ranges,
                 const std::optional<int32_t>&                                limit,
                 const revision&                                              peg_revision,
                 bool                                                         discover_changed_paths,
                 bool                                                         strict_node_history,
                 bool                                                         include_merged_revisions,
                 const std::optional<const std::vector<std::string>>&         revprops) const {
    child_pool pool(_pool);

    auto raw_paths          = convert_from_vector(paths, pool, true);
    auto raw_peg_revision   = convert_from_revision(peg_revision);
    auto raw_revision_rangs = convert_from_revision_ranges(revision_ranges, pool);
    auto raw_limit          = limit ? *limit : 0;
    auto raw_revprops       = convert_from_vector(revprops, pool);
    auto callback_ref       = std::cref(callback);

    check_result(svn_client_log5(raw_paths,
                                 &raw_peg_revision,
                                 raw_revision_rangs,
                                 raw_limit,
                                 discover_changed_paths,
                                 strict_node_history,
                                 include_merged_revisions,
                                 raw_revprops,
                                 invoke_log,
                                 &callback_ref,
                                 _context,
                                 pool));
}

void client::remove(const std::vector<std::string>& paths,
                    const remove_callback&          callback,
                    bool                            force,
                    bool                            keep_local,
                    const string_map&               revprop_table) const {
    child_pool pool(_pool);

    auto raw_paths    = convert_from_vector(paths, pool, true);
    auto callback_ref = std::cref(callback);
    auto raw_props    = convert_from_map(revprop_table, _pool);

    check_result(svn_client_delete4(raw_paths,
                                    force,
                                    keep_local,
                                    raw_props,
                                    invoke_commit,
                                    &callback_ref,
                                    _context,
                                    pool));
}

void client::resolve(const std::string& path,
                     svn::depth         depth,
                     conflict_choose    choose) const {
    child_pool pool(_pool);

    auto raw_path = convert_from_path(path, pool);

    check_result(svn_client_resolve(raw_path,
                                    static_cast<svn_depth_t>(depth),
                                    static_cast<svn_wc_conflict_choice_t>(choose),
                                    _context,
                                    pool));
}

void client::revert(const std::vector<std::string>&                      paths,
                    svn::depth                                           depth,
                    const std::optional<const std::vector<std::string>>& changelists,
                    bool                                                 clear_changelists,
                    bool                                                 metadata_only,
                    bool                                                 added_keep_local) const {
    child_pool pool(_pool);

    auto raw_paths       = convert_from_vector(paths, pool, true);
    auto raw_changelists = convert_from_vector(changelists, pool);

    check_result(svn_client_revert4(raw_paths,
                                    static_cast<svn_depth_t>(depth),
                                    raw_changelists,
                                    clear_changelists,
                                    metadata_only,
                                    added_keep_local,
                                    _context,
                                    pool));
}

static svn_error_t* invoke_status(void*                      raw_baton,
                                  const char*                path,
                                  const svn_client_status_t* raw_status,
                                  apr_pool_t*                raw_scratch_pool) {
    auto callback = get_reference<client::status_callback>(raw_baton);
    callback(path, convert_to_status(raw_status));
    return nullptr;
}

int32_t client::status(const std::string&                                   path,
                       const status_callback&                               callback,
                       const revision&                                      revision,
                       svn::depth                                           depth,
                       bool                                                 get_all,
                       bool                                                 check_out_of_date,
                       bool                                                 check_working_copy,
                       bool                                                 no_ignore,
                       bool                                                 ignore_externals,
                       bool                                                 depth_as_sticky,
                       const std::optional<const std::vector<std::string>>& changelists) const {
    child_pool pool(_pool);

    auto raw_path        = convert_from_path(path, pool);
    auto callback_ref    = std::cref(callback);
    auto raw_revision    = convert_from_revision(revision);
    auto raw_changelists = convert_from_vector(changelists, pool);

    int32_t result_rev;

    check_result(svn_client_status6(reinterpret_cast<svn_revnum_t*>(&result_rev),
                                    _context,
                                    raw_path,
                                    &raw_revision,
                                    static_cast<svn_depth_t>(depth),
                                    get_all,
                                    check_out_of_date,
                                    check_working_copy,
                                    no_ignore,
                                    ignore_externals,
                                    depth_as_sticky,
                                    raw_changelists,
                                    invoke_status,
                                    &callback_ref,
                                    pool));

    return result_rev;
}

int32_t client::update(const std::string& path,
                       const revision&    revision,
                       svn::depth         depth,
                       bool               depth_is_sticky,
                       bool               ignore_externals,
                       bool               allow_unver_obstructions,
                       bool               adds_as_modification,
                       bool               make_parents) const {
    child_pool pool(_pool);

    auto raw_paths    = convert_from_vector({path}, pool);
    auto raw_revision = convert_from_revision(revision);

    apr_array_header_t* raw_result_revs;

    check_result(svn_client_update4(&raw_result_revs,
                                    raw_paths,
                                    &raw_revision,
                                    static_cast<svn_depth_t>(depth),
                                    depth_is_sticky,
                                    ignore_externals,
                                    allow_unver_obstructions,
                                    adds_as_modification,
                                    make_parents,
                                    _context,
                                    pool));

    return APR_ARRAY_IDX(raw_result_revs, 0, int32_t);
}

std::vector<int32_t> client::update(const std::vector<std::string>& paths,
                                    const revision&                 revision,
                                    svn::depth                      depth,
                                    bool                            depth_is_sticky,
                                    bool                            ignore_externals,
                                    bool                            allow_unver_obstructions,
                                    bool                            adds_as_modification,
                                    bool                            make_parents) const {
    child_pool pool(_pool);

    auto raw_paths    = convert_from_vector(paths, pool, true);
    auto raw_revision = convert_from_revision(revision);

    apr_array_header_t* raw_result_revs;

    check_result(svn_client_update4(&raw_result_revs,
                                    raw_paths,
                                    &raw_revision,
                                    static_cast<svn_depth_t>(depth),
                                    depth_is_sticky,
                                    ignore_externals,
                                    allow_unver_obstructions,
                                    adds_as_modification,
                                    make_parents,
                                    _context,
                                    pool));

    auto result = std::vector<int32_t>(raw_result_revs->nelts);
    for (int i = 0; i < raw_result_revs->nelts; i++)
        result[i] = APR_ARRAY_IDX(raw_result_revs, i, int32_t);
    return result;
}

std::string client::get_working_copy_root(const std::string& path) const {
    child_pool pool(_pool);

    auto raw_path = convert_from_path(path, pool);

    const char* raw_result;

    check_result(svn_client_get_wc_root(&raw_result, raw_path, _context, pool, pool));

    return std::string(raw_result);
}
} // namespace svn
