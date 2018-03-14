#pragma once

#include <optional>
#include <string>

#include <apr_pools.h>
#include <svn_client.h>
#include <svn_opt.h>
#include <svn_path.h>

#include <cpp/svn_type_error.hpp>
#include <cpp/types.hpp>

static svn::svn_error* copy_error(svn_error_t* error) {
    const auto buffer_size = 100;
    char       buffer[buffer_size];
    auto       message = svn_err_best_message(error, buffer, buffer_size);

    return new svn::svn_error(error->apr_err,
                              message,
                              error->child ? copy_error(error->child) : nullptr,
                              error->file ? std::string(error->file) : std::string(),
                              error->line);
}

static svn::svn_error copy_error(svn_error_t& error) {
    const auto buffer_size = 100;
    char       buffer[buffer_size];
    auto       message = svn_err_best_message(&error, buffer, buffer_size);

    return svn::svn_error(error.apr_err,
                          message,
                          error.child ? copy_error(error.child) : nullptr,
                          error.file ? std::string(error.file) : std::string(),
                          error.line);
}

static void throw_error(svn_error_t* error) {
    auto purged = svn_error_purge_tracing(error);
    auto result = copy_error(*purged);
    svn_error_clear(error);
    throw result;
}

static void check_result(apr_status_t status) {
    if (status != 0)
        throw_error(svn_error_create(status, nullptr, nullptr));
}

static void check_result(svn_error_t* result) {
    if (result != nullptr)
        throw_error(result);
}

static void check_string(const std::string& value) {
    if (value.size() == 0)
        throw svn::svn_type_error("");

    if (value.find('\0') != std::string::npos)
        throw svn::svn_type_error("");
}

static const char* convert_from_string(const std::string& value) {
    check_string(value);
    return value.c_str();
}

static const char* convert_from_path(const std::string& value,
                                     apr_pool_t*        pool) {
    auto raw = convert_from_string(value);

    if (svn_path_is_url(raw)) {
        throw svn::svn_type_error("");
    }

    check_result(svn_dirent_get_absolute(&raw, raw, pool));
    return raw;
}

static const char* convert_from_url(const std::string& value,
                                    apr_pool_t*        pool) {
    auto raw = convert_from_string(value);

    if (!svn_path_is_url(raw)) {
        throw svn::svn_type_error("");
    }

    // svn requires the drive letter on Windows to be lowercased
    // `svn_uri_canonicalize` will do it for us
    return svn_uri_canonicalize(raw, pool);
}

static const char* convert_from_path(const std::optional<const std::string>& value,
                                     apr_pool_t*                             pool) {
    if (!value) {
        return nullptr;
    }

    return convert_from_path(*value, pool);
}

static const apr_array_header_t* convert_from_vector(const svn::string_vector& value,
                                                     apr_pool_t*               pool,
                                                     bool                      allowEmpty,
                                                     bool                      isPath) {
    if (value.size() == 0) {
        if (allowEmpty)
            return nullptr;
        else
            throw svn::svn_type_error("");
    }

    auto result = apr_array_make(pool, static_cast<int>(value.size()), sizeof(const char*));

    for (auto item : value) {
        auto converted                      = isPath ? convert_from_path(item, pool) : convert_from_string(item);
        APR_ARRAY_PUSH(result, const char*) = converted;
    }

    return result;
}

static const apr_array_header_t* convert_from_vector(const std::string& value,
                                                     apr_pool_t*        pool) {
    auto result = apr_array_make(pool, 1, sizeof(const char*));

    auto raw_path                       = convert_from_path(value, pool);
    APR_ARRAY_PUSH(result, std::string) = raw_path;

    return result;
}

static const char* duplicate_string(apr_pool_t* pool, const std::string& string) {
    return static_cast<const char*>(apr_pmemdup(pool, string.c_str(), string.size() + 1));
}

static apr_hash_t* convert_from_map(const svn::string_map& map, apr_pool_t* pool) {
    if (map.size() == 0)
        return nullptr;

    auto result = apr_hash_make(pool);

    for (auto pair : map) {
        auto key     = pair.first;
        auto raw_key = duplicate_string(pool, key);

        auto value     = pair.second;
        auto duplicate = duplicate_string(pool, value);
        auto raw_value = new svn_string_t{duplicate, value.size()};

        apr_hash_set(result, raw_key, key.size(), raw_value);
    }

    return result;
}

static svn_opt_revision_t convert_from_revision(const svn::revision& value) {
    auto result       = svn_opt_revision_t();
    result.kind       = static_cast<svn_opt_revision_kind>(value.kind);
    result.value.date = static_cast<apr_time_t>(value.date);
    return result;
}

static std::optional<int32_t> convert_to_revision_number(svn_revnum_t value) {
    if (value != 0)
        return static_cast<int32_t>(value);

    return {};
}

static std::optional<std::string> convert_to_string(const char* value) {
    if (value)
        return value;

    return {};
}

static std::optional<svn::lock> convert_to_lock(const svn_lock_t* raw) {
    if (raw == nullptr)
        return {};

    auto result            = svn::lock();
    result.comment         = raw->comment;
    result.creation_date   = raw->creation_date;
    result.expiration_date = raw->expiration_date;
    result.is_dav_comment  = raw->is_dav_comment;
    result.owner           = raw->owner;
    result.path            = raw->path;
    result.token           = raw->token;

    return result;
}

static svn::status convert_to_status(const svn_client_status_t* raw) {
    if (raw == nullptr)
        throw svn::svn_type_error("");

    return svn::status{
        static_cast<svn::node_kind>(raw->kind),
        raw->local_abspath,
        raw->filesize,
        static_cast<bool>(raw->versioned),
        static_cast<bool>(raw->conflicted),
        static_cast<svn::status_kind>(raw->node_status),
        static_cast<svn::status_kind>(raw->text_status),
        static_cast<svn::status_kind>(raw->prop_status),
        static_cast<bool>(raw->wc_is_locked),
        static_cast<bool>(raw->copied),
        raw->repos_root_url,
        raw->repos_uuid,
        raw->repos_relpath,
        raw->revision,
        raw->changed_rev,
        raw->changed_date,
        raw->changed_author,
        static_cast<bool>(raw->switched),
        static_cast<bool>(raw->file_external),
        convert_to_lock(raw->lock),
        raw->changelist,
        static_cast<svn::depth>(raw->depth),
        static_cast<svn::node_kind>(raw->ood_kind),
        static_cast<svn::status_kind>(raw->repos_node_status),
        static_cast<svn::status_kind>(raw->repos_text_status),
        static_cast<svn::status_kind>(raw->repos_prop_status),
        convert_to_lock(raw->repos_lock),
        raw->ood_changed_rev,
        raw->ood_changed_date,
        raw->ood_changed_author,
        raw->moved_from_abspath,
        raw->moved_to_abspath};
}

static std::optional<svn::checksum> convert_to_checksum(const svn_checksum_t* raw) {
    if (raw == nullptr)
        return {};

    return svn::checksum{
        raw->digest,
        static_cast<svn::checksum_kind>(raw->kind)};
}

static std::optional<svn::working_copy_info> convert_to_working_copy_info(const svn_wc_info_t* raw) {
    if (raw == nullptr)
        return {};

    return svn::working_copy_info{
        raw->copyfrom_url,
        raw->copyfrom_rev,
        convert_to_checksum(raw->checksum),
        raw->changelist,
        static_cast<svn::depth>(raw->depth),
        raw->recorded_size,
        raw->recorded_time,
        raw->wcroot_abspath,
        raw->moved_from_abspath,
        raw->moved_to_abspath};
}

static svn::info convert_to_info(const svn_client_info2_t* raw) {
    if (raw == nullptr)
        throw svn::svn_type_error("");

    return svn::info{
        raw->URL,
        raw->rev,
        raw->repos_root_URL,
        raw->repos_UUID,
        static_cast<svn::node_kind>(raw->kind),
        raw->size,
        raw->last_changed_rev,
        raw->last_changed_date,
        raw->last_changed_author,
        convert_to_lock(raw->lock),
        convert_to_working_copy_info(raw->wc_info)};
}

static svn::commit_info convert_to_commit_info(const svn_commit_info_t* raw) {
    if (raw == nullptr)
        throw svn::svn_type_error("");

    return svn::commit_info{
        static_cast<int32_t>(raw->revision),
        raw->date,
        raw->author,
        raw->post_commit_err,
        raw->repos_root};
}
