#pragma region Editor Support
#ifndef CLASS_NAME
#include "../node_client.hpp"

#define EXPORT_NAME "Client"
#define ASYNC false

#define METHOD_BEGIN(name)                                               \
    void client::name(const v8::FunctionCallbackInfo<v8::Value>& args) { \
        auto isolate = args.GetIsolate();                                \
        auto context = isolate->GetCurrentContext();                     \
        try {                                                            \
            auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

#define CONVERT_CALLBACK(callback) \
    callback

template <class T>
class future {
  public:
    future() {}

    T value;

    T get() {
        return value;
    }
};

template <>
class future<void> {
  public:
    future() {}

    void get() {}
};

#define ASYNC_BEGIN(result, ...) \
    future<result> future;

#define ASYNC_RETURN(result) \
    future.value = result;

#define ASYNC_END(...)

#define ASYNC_RESULT \
    future.get()

#define METHOD_RETURN(value) \
    args.GetReturnValue().Set(value);

#define METHOD_END                                                                         \
    }                                                                                      \
    catch (svn::svn_type_error & error) {                                                  \
        isolate->ThrowException(v8::Exception::TypeError(v8::New(isolate, error.what()))); \
    }                                                                                      \
    catch (svn::svn_error & raw_error) {                                                   \
        auto error = copy_error(isolate, raw_error);                                       \
        isolate->ThrowException(error);                                                    \
    }                                                                                      \
    }
#endif
#pragma endregion Editor Support

#include <cstring>

#include <node_buffer.h>

#include <cpp/client.hpp>
#include <cpp/svn_type_error.hpp>

#include <node/v8.hpp>

#define INTERNALIZED_STRING(value) \
    v8::New(isolate, value, v8::NewStringType::kInternalized, sizeof(value) - 1)

static std::string convert_string(const v8::Local<v8::Value>& value) {
    if (!value->IsString())
        throw svn::svn_type_error("");

    v8::String::Utf8Value utf8(value);
    auto                  length = static_cast<size_t>(utf8.length());

    if (std::strlen(*utf8) != length)
        throw svn::svn_type_error("");

    return std::string(*utf8, length);
}

static v8::Local<v8::Value> copy_error(v8::Isolate* isolate, svn::svn_error& raw_error) {
    auto error = v8::Exception::Error(v8::New(isolate, raw_error.what()));
    if (raw_error.child != nullptr)
        error.As<v8::Object>()->Set(INTERNALIZED_STRING("child"), copy_error(isolate, *raw_error.child));
    return error;
}

static std::vector<std::string> convert_array(const v8::Local<v8::Value>& value, bool allowEmpty) {
    if (value->IsUndefined()) {
        if (allowEmpty)
            return std::vector<std::string>();
        else
            throw svn::svn_type_error("");
    }

    if (value->IsString())
        return std::vector<std::string>{convert_string(value)};

    if (value->IsArray()) {
        auto array  = value.As<v8::Array>();
        auto length = array->Length();
        auto result = std::vector<std::string>();
        for (uint32_t i = 0; i < length; i++) {
            auto item = array->Get(i);
            result.push_back(std::move(convert_string(item)));
        }
        return result;
    }

    throw svn::svn_type_error("");
}

static v8::Local<v8::Object> convert_options(const v8::Local<v8::Value> options) {
    if (options->IsUndefined())
        return v8::Local<v8::Object>();

    if (options->IsObject())
        return options.As<v8::Object>();

    throw svn::svn_type_error("");
}

static svn::revision convert_revision(v8::Isolate*                 isolate,
                                      const v8::Local<v8::Object>& options,
                                      const char*                  key,
                                      svn::revision_kind           defaultValue) {
    if (options.IsEmpty())
        return defaultValue;

    auto value = options->Get(v8::New(isolate, key, v8::NewStringType::kInternalized));
    if (value->IsUndefined())
        return defaultValue;

    if (value->IsNumber()) {
        auto simple = static_cast<svn::revision_kind>(value->Int32Value());
        return simple;
    }

    if (value->IsObject()) {
        auto object = value.As<v8::Object>();
        auto number = object->Get(INTERNALIZED_STRING("number"));
        if (!number->IsUndefined()) {
            if (!number->IsNumber())
                throw svn::svn_type_error("");

            return svn::revision(number->Int32Value());
        }

        auto date = object->Get(INTERNALIZED_STRING("date"));
        if (!date->IsUndefined()) {
            if (!date->IsNumber())
                throw svn::svn_type_error("");

            return svn::revision(date->IntegerValue());
        }
    }

    throw svn::svn_type_error("");
}

static svn::depth convert_depth(v8::Isolate*                 isolate,
                                const v8::Local<v8::Object>& options,
                                const char*                  key,
                                svn::depth                   defaultValue) {
    if (options.IsEmpty())
        return defaultValue;

    auto value = options->Get(v8::New(isolate, key, v8::NewStringType::kInternalized));
    if (value->IsUndefined())
        return defaultValue;

    if (value->IsNumber())
        return static_cast<svn::depth>(value->Int32Value());

    throw svn::svn_type_error("");
}

static bool convert_boolean(v8::Isolate*                 isolate,
                            const v8::Local<v8::Object>& options,
                            const char*                  key,
                            bool                         defaultValue) {
    if (options.IsEmpty())
        return defaultValue;

    auto value = options->Get(v8::New(isolate, key, v8::NewStringType::kInternalized));
    if (value->IsUndefined())
        return defaultValue;

    if (value->IsBoolean())
        return value->BooleanValue();

    throw svn::svn_type_error("");
}

static std::vector<std::string> convert_array(v8::Isolate*          isolate,
                                              v8::Local<v8::Object> options,
                                              const char*           key) {
    if (options.IsEmpty())
        return std::vector<std::string>();

    auto value = options->Get(v8::New(isolate, key, v8::NewStringType::kInternalized));
    if (value->IsUndefined())
        return std::vector<std::string>();

    return convert_array(value, true);
}

static void buffer_free_pointer(char*, void* hint) {
    delete static_cast<std::vector<char>*>(hint);
}

static v8::Local<v8::Object> buffer_from_vector(v8::Isolate* isolate, std::vector<char>& vector) {
    auto pointer = new std::vector<char>(std::move(vector));
    return node::Buffer::New(isolate,
                             pointer->data(),
                             pointer->size(),
                             buffer_free_pointer,
                             pointer)
        .ToLocalChecked();
}

#define STRINGIFY_INTERNAL(X) #X
#define STRINGIFY(X) STRINGIFY_INTERNAL(X)

#define SET_READ_ONLY(object, name, value)                 \
    (object)->DefineOwnProperty(context,                   \
                                INTERNALIZED_STRING(name), \
                                value,                     \
                                v8::PropertyAttributeEx::ReadOnlyDontDelete)

#define SET_PROTOTYPE_METHOD(signature, prototype, name, callback, length)                    \
    /* Add a scope to hide extra variables */                                                 \
    {                                                                                         \
        auto function = v8::FunctionTemplate::New(isolate,                /* isolate */       \
                                                  callback,               /* callback */      \
                                                  v8::Local<v8::Value>(), /* data */          \
                                                  signature,              /* signature */     \
                                                  length);                /* length */        \
        function->RemovePrototype();                                                          \
        prototype->Set(INTERNALIZED_STRING(name), function, v8::PropertyAttribute::DontEnum); \
    }

#define CONVERT_OPTIONS_AND_CALLBACK(index)                \
    v8::Local<v8::Object>   options;                       \
    v8::Local<v8::Function> raw_callback;                  \
    if (args[index]->IsFunction()) {                       \
        raw_callback = args[index].As<v8::Function>();     \
    } else if (args[index + 1]->IsFunction()) {            \
        options      = convert_options(args[index]);       \
        raw_callback = args[index + 1].As<v8::Function>(); \
    } else {                                               \
        throw svn::svn_type_error("");                     \
    }                                                      \
    auto _raw_callback = std::make_shared<v8::Global<v8::Function>>(isolate, raw_callback);

namespace node {
void CLASS_NAME::init(v8::Local<v8::Object>&  exports,
                      v8::Isolate*            isolate,
                      v8::Local<v8::Context>& context) {
    auto client    = v8::New<v8::FunctionTemplate>(isolate, create_instance);
    auto signature = v8::Signature::New(isolate, client);

    client->SetClassName(INTERNALIZED_STRING(EXPORT_NAME));
    client->ReadOnlyPrototype();

    client->InstanceTemplate()->SetInternalFieldCount(1);

    auto prototype = client->PrototypeTemplate();
    SET_PROTOTYPE_METHOD(signature, prototype, "add_simple_auth_provider", add_simple_auth_provider, 1);
    SET_PROTOTYPE_METHOD(signature, prototype, "remove_simple_auth_provider", remove_simple_auth_provider, 1);

    SET_PROTOTYPE_METHOD(signature, prototype, "add_to_changelist", add_to_changelist, 2);
    SET_PROTOTYPE_METHOD(signature, prototype, "get_changelists", get_changelists, 2);
    SET_PROTOTYPE_METHOD(signature, prototype, "remove_from_changelists", remove_from_changelists, 2);

    SET_PROTOTYPE_METHOD(signature, prototype, "add", add, 1);
    SET_PROTOTYPE_METHOD(signature, prototype, "cat", cat, 1);
    SET_PROTOTYPE_METHOD(signature, prototype, "checkout", checkout, 2);
    SET_PROTOTYPE_METHOD(signature, prototype, "cleanup", cleanup, 1);
    SET_PROTOTYPE_METHOD(signature, prototype, "commit", commit, 3);
    SET_PROTOTYPE_METHOD(signature, prototype, "info", info, 2);
    SET_PROTOTYPE_METHOD(signature, prototype, "remove", remove, 2);
    SET_PROTOTYPE_METHOD(signature, prototype, "resolve", resolve, 1);
    SET_PROTOTYPE_METHOD(signature, prototype, "revert", revert, 1);
    SET_PROTOTYPE_METHOD(signature, prototype, "status", status, 2);
    SET_PROTOTYPE_METHOD(signature, prototype, "update", update, 1);

    SET_PROTOTYPE_METHOD(signature, prototype, "get_working_copy_root", get_working_copy_root, 1);

    SET_READ_ONLY(exports, EXPORT_NAME, client->GetFunction());
}

void CLASS_NAME::create_instance(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    if (!args.IsConstructCall()) {
        auto message = "Class constructor " STRINGIFY(CLASS_NAME) " cannot be invoked without 'new'";
        isolate->ThrowException(v8::Exception::TypeError(v8::New(isolate, message)));
        return;
    }

    std::optional<const std::string> config_path;
    if (args[0]->IsString()) {
        config_path.emplace(convert_string(args[0]));
    }

    auto result = new CLASS_NAME(isolate, config_path);
    result->Wrap(args.This());
}

void CLASS_NAME::add_simple_auth_provider(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();

    auto _this = node::ObjectWrap::Unwrap<CLASS_NAME>(args.Holder());

    if (!args[0]->IsFunction()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::Empty(isolate)));
        return;
    }

    _this->_simple_auth_provider.add(args[0].As<v8::Function>());
}

void CLASS_NAME::remove_simple_auth_provider(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();

    auto _this = node::ObjectWrap::Unwrap<CLASS_NAME>(args.Holder());

    if (!args[0]->IsFunction()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::Empty(isolate)));
        return;
    }

    _this->_simple_auth_provider.remove(args[0].As<v8::Function>());
}

METHOD_BEGIN(add_to_changelist)
    auto paths      = convert_array(args[0], false);
    auto changelist = convert_string(args[1]);

    auto options     = convert_options(args[2]);
    auto depth       = convert_depth(isolate, options, "depth", svn::depth::infinity);
    auto changelists = convert_array(isolate, options, "changelists");

    ASYNC_BEGIN(void, paths, changelist, depth, changelists)
        _this->_client->add_to_changelist(paths, changelist, depth, changelists);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate))
METHOD_END

METHOD_BEGIN(get_changelists)
    auto path = convert_string(args[0]);

    CONVERT_OPTIONS_AND_CALLBACK(1)

    auto _callback = [isolate, _raw_callback](const char* path, const char* changelist) -> void {
        v8::HandleScope scope(isolate);

        const auto           argc       = 2;
        v8::Local<v8::Value> argv[argc] = {
            v8::New(isolate, path),
            v8::New(isolate, changelist)};

        auto callback = _raw_callback->Get(isolate);
        callback->Call(v8::Undefined(isolate), argc, argv);
    };
    auto callback = CONVERT_CALLBACK(_callback);

    auto depth       = convert_depth(isolate, options, "depth", svn::depth::infinity);
    auto changelists = convert_array(isolate, options, "changelists");

    ASYNC_BEGIN(void, path, callback, depth, changelists)
        _this->_client->get_changelists(path, callback, depth, changelists);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(remove_from_changelists)
    auto paths = convert_array(args[0], false);

    auto options     = convert_options(args[1]);
    auto depth       = convert_depth(isolate, options, "depth", svn::depth::infinity);
    auto changelists = convert_array(isolate, options, "changelists");

    ASYNC_BEGIN(void, paths, depth, changelists)
        _this->_client->remove_from_changelists(paths, depth, changelists);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(add)
    auto path = convert_string(args[0]);

    auto options = convert_options(args[1]);
    auto depth   = convert_depth(isolate, options, "depth", svn::depth::infinity);

    ASYNC_BEGIN(void, path, depth)
        _this->_client->add(path, depth);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(cat)
    auto path = convert_string(args[0]);

    auto options      = convert_options(args[1]);
    auto peg_revision = convert_revision(isolate, options, "peg_revision", svn::revision_kind::working);
    auto revision     = convert_revision(isolate, options, "revision", svn::revision_kind::working);

    ASYNC_BEGIN(svn::cat_result, path, peg_revision, revision)
        ASYNC_RETURN(_this->_client->cat(path, peg_revision, revision));
    ASYNC_END()

    auto raw_result = ASYNC_RESULT;

    auto result = v8::New<v8::Object>(isolate);
    result->Set(INTERNALIZED_STRING("content"), buffer_from_vector(isolate, raw_result.content));

    auto properties = v8::New<v8::Object>(isolate);
    for (auto pair : raw_result.properties) {
        properties->Set(v8::New(isolate, pair.first), v8::New(isolate, pair.second));
    }
    result->Set(INTERNALIZED_STRING("properties"), properties);

    METHOD_RETURN(result);
METHOD_END

METHOD_BEGIN(checkout)
    auto url  = convert_string(args[0]);
    auto path = convert_string(args[1]);

    auto options      = convert_options(args[2]);
    auto peg_revision = convert_revision(isolate, options, "peg_revision", svn::revision_kind::head);
    auto revision     = convert_revision(isolate, options, "revision", svn::revision_kind::head);
    auto depth        = convert_depth(isolate, options, "depth", svn::depth::infinity);

    ASYNC_BEGIN(int32_t, url, path, peg_revision, revision, depth)
        ASYNC_RETURN(_this->_client->checkout(url, path, peg_revision, revision, depth));
    ASYNC_END()

    auto result = ASYNC_RESULT;
    METHOD_RETURN(v8::New(isolate, result));
METHOD_END

METHOD_BEGIN(cleanup)
    auto path = convert_string(args[0]);

    ASYNC_BEGIN(void, path)
        _this->_client->cleanup(path, true, true, true, true, true);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

static decltype(auto) convert_commit_callback(v8::Isolate* isolate, const v8::Local<v8::Value>& value) {
    if (!value->IsFunction())
        throw svn::svn_type_error("");

    auto raw_callback  = value.As<v8::Function>();
    auto _raw_callback = std::make_shared<v8::Global<v8::Function>>(isolate, raw_callback);
    auto _callback     = [isolate, _raw_callback](const svn::commit_info* raw_commit) -> void {
        v8::HandleScope scope(isolate);

        auto commit = v8::New<v8::Object>(isolate);
        commit->Set(INTERNALIZED_STRING("author"), v8::New(isolate, raw_commit->author));
        commit->Set(INTERNALIZED_STRING("date"), v8::New(isolate, raw_commit->date));
        commit->Set(INTERNALIZED_STRING("repos_root"), v8::New(isolate, raw_commit->repos_root));
        commit->Set(INTERNALIZED_STRING("revision"), v8::New(isolate, raw_commit->revision));

        if (raw_commit->post_commit_error != nullptr)
            commit->Set(INTERNALIZED_STRING("post_commit_error"), v8::New(isolate, raw_commit->post_commit_error));

        const auto           argc       = 1;
        v8::Local<v8::Value> argv[argc] = {commit};

        auto callback = _raw_callback->Get(isolate);
        callback->Call(v8::Undefined(isolate), argc, argv);
    };

    return CONVERT_CALLBACK(_callback);
}

METHOD_BEGIN(commit)
    auto paths    = convert_array(args[0], false);
    auto message  = convert_string(args[1]);
    auto callback = convert_commit_callback(isolate, args[2]);

    ASYNC_BEGIN(void, paths, message, callback)
        _this->_client->commit(paths, message, callback);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

static auto convert_to_date(v8::Local<v8::Context>& context, int64_t value) {
    auto d = static_cast<double>(value / 1000);
    return v8::Date::New(context, d).ToLocalChecked();
}

METHOD_BEGIN(info)
    auto path = convert_string(args[0]);

    CONVERT_OPTIONS_AND_CALLBACK(1)

    auto _callback = [isolate, _raw_callback](const char* path, const svn::info& raw_info) -> void {
        v8::HandleScope scope(isolate);

        auto context = isolate->GetCurrentContext();

        auto info = v8::New<v8::Object>(isolate);
        info->Set(INTERNALIZED_STRING("path"), v8::New(isolate, path));
        info->Set(INTERNALIZED_STRING("kind"), v8::New(isolate, static_cast<int32_t>(raw_info.kind)));
        info->Set(INTERNALIZED_STRING("last_changed_author"), v8::New(isolate, raw_info.last_changed_author));
        info->Set(INTERNALIZED_STRING("last_changed_date"), convert_to_date(context, raw_info.last_changed_date));
        info->Set(INTERNALIZED_STRING("last_changed_rev"), v8::New(isolate, static_cast<int32_t>(raw_info.last_changed_rev)));
        info->Set(INTERNALIZED_STRING("repos_root_url"), v8::New(isolate, raw_info.repos_root_url));
        info->Set(INTERNALIZED_STRING("repos_root_uuid"), v8::New(isolate, raw_info.repos_uuid));
        info->Set(INTERNALIZED_STRING("url"), v8::New(isolate, raw_info.url));

        const auto           argc       = 1;
        v8::Local<v8::Value> argv[argc] = {info};

        auto callback = _raw_callback->Get(isolate);
        callback->Call(v8::Undefined(isolate), argc, argv);
    };
    auto callback = CONVERT_CALLBACK(_callback);

    auto peg_revision = convert_revision(isolate, options, "peg_revision", svn::revision_kind::unspecified);
    auto revision     = convert_revision(isolate, options, "revision", svn::revision_kind::unspecified);
    auto depth        = convert_depth(isolate, options, "depth", svn::depth::empty);

    ASYNC_BEGIN(void, path, callback, peg_revision, revision, depth)
        _this->_client->info(path, callback, peg_revision, revision, depth);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(remove)
    auto paths    = convert_array(args[0], false);
    auto callback = convert_commit_callback(isolate, args[1]);

    ASYNC_BEGIN(void, paths, callback)
        _this->_client->remove(paths, callback);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(resolve)
    auto path = convert_string(args[0]);

    ASYNC_BEGIN(void, path)
        _this->_client->resolve(path);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(revert)
    auto paths = convert_array(args[0], false);

    ASYNC_BEGIN(void, paths)
        _this->_client->revert(paths);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

static v8::Local<v8::Value> copy_string(v8::Isolate* isolate, const char* value) {
    if (value == nullptr)
        return v8::Undefined(isolate);
    return v8::New(isolate, value);
}

METHOD_BEGIN(status)
    auto path = convert_string(args[0]);

    CONVERT_OPTIONS_AND_CALLBACK(1)

    auto _callback = [isolate, _raw_callback](const std::string& path, const svn::status& raw_status) -> void {
        v8::HandleScope scope(isolate);

        auto context = isolate->GetCurrentContext();

        auto status = v8::New<v8::Object>(isolate);
        status->Set(INTERNALIZED_STRING("path"), v8::New(isolate, path));
        status->Set(INTERNALIZED_STRING("changelist"), copy_string(isolate, raw_status.changelist));
        status->Set(INTERNALIZED_STRING("changed_author"), copy_string(isolate, raw_status.changed_author));
        status->Set(INTERNALIZED_STRING("changed_date"), convert_to_date(context, raw_status.changed_date));
        status->Set(INTERNALIZED_STRING("changed_rev"), v8::New(isolate, raw_status.changed_rev));
        status->Set(INTERNALIZED_STRING("conflicted"), v8::New(isolate, raw_status.conflicted));
        status->Set(INTERNALIZED_STRING("copied"), v8::New(isolate, raw_status.copied));
        status->Set(INTERNALIZED_STRING("depth"), v8::New(isolate, static_cast<int32_t>(raw_status.node_depth)));
        status->Set(INTERNALIZED_STRING("file_external"), v8::New(isolate, raw_status.file_external));
        status->Set(INTERNALIZED_STRING("kind"), v8::New(isolate, static_cast<int32_t>(raw_status.kind)));
        status->Set(INTERNALIZED_STRING("node_status"), v8::New(isolate, static_cast<int32_t>(raw_status.node_status)));
        status->Set(INTERNALIZED_STRING("prop_status"), v8::New(isolate, static_cast<int32_t>(raw_status.prop_status)));
        status->Set(INTERNALIZED_STRING("revision"), v8::New(isolate, static_cast<int32_t>(raw_status.revision)));
        status->Set(INTERNALIZED_STRING("text_status"), v8::New(isolate, static_cast<int32_t>(raw_status.text_status)));
        status->Set(INTERNALIZED_STRING("versioned"), v8::New(isolate, raw_status.versioned));

        const auto           argc       = 1;
        v8::Local<v8::Value> argv[argc] = {status};

        auto callback = _raw_callback->Get(isolate);
        callback->Call(v8::Undefined(isolate), argc, argv);
    };
    auto callback = CONVERT_CALLBACK(_callback);

    auto revision         = convert_revision(isolate, options, "revision", svn::revision_kind::working);
    auto depth            = convert_depth(isolate, options, "depth", svn::depth::infinity);
    auto ignore_externals = convert_boolean(isolate, options, "ignore_externals", false);

    ASYNC_BEGIN(void, path, callback, revision, depth, ignore_externals)
        _this->_client->status(path, callback, revision, depth, false, false, true, false, ignore_externals);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(update)
    auto paths  = convert_array(args[0], false);
    auto single = args[0]->IsString();

    ASYNC_BEGIN(std::vector<int32_t>, paths)
        ASYNC_RETURN(_this->_client->update(paths));
    ASYNC_END(single)

    if (single) {
        auto result = v8::New(isolate, ASYNC_RESULT[0]);
        METHOD_RETURN(result);
    } else {
        auto vector = ASYNC_RESULT;
        auto result = v8::New<v8::Array>(isolate, static_cast<int32_t>(vector.size()));
        for (uint32_t i = 0; i < vector.size(); i++)
            result->Set(i, v8::New(isolate, vector[i]));
        METHOD_RETURN(result);
    }
METHOD_END

METHOD_BEGIN(get_working_copy_root)
    auto path = convert_string(args[0]);

    ASYNC_BEGIN(std::string, path)
        ASYNC_RETURN(_this->_client->get_working_copy_root(path));
    ASYNC_END()

    auto result = v8::New(isolate, ASYNC_RESULT);
    METHOD_RETURN(result);
METHOD_END

CLASS_NAME::CLASS_NAME(v8::Isolate* isolate, const std::optional<const std::string>& config_path)
    : _client(new svn::client(config_path))
    , _simple_auth_provider(isolate, ASYNC) {
    _client->add_simple_auth_provider(std::make_shared<svn::client::simple_auth_provider::element_type>(_simple_auth_provider));
}

CLASS_NAME::~CLASS_NAME() {}

} // namespace node
