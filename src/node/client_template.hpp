#include <node_buffer.h>

#include <svn_types.h>

#include <cpp/client.hpp>
#include <cpp/svn_type_error.hpp>

#include <node/v8.hpp>

static std::string convert_string(const v8::Local<v8::Value>& value) {
    if (value.IsEmpty())
        throw svn::svn_type_error("");

    if (!value->IsString())
        throw svn::svn_type_error("");

    v8::String::Utf8Value utf8(value);
    auto                  length = utf8.length();

    if (strlen(*utf8) != length)
        throw svn::svn_type_error("");

    return std::string(*utf8, utf8.length());
}

static std::vector<std::string> convert_array(const v8::Local<v8::Value>& value, bool allowEmpty) {
    if (value.IsEmpty() || value->IsUndefined()) {
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
        auto result = std::vector<std::string>(length);
        for (uint32_t i = 0; i < length; i++) {
            auto item = array->Get(i);
            result.push_back(std::move(convert_string(item)));
        }
        return result;
    }

    throw svn::svn_type_error("");
}

static svn_opt_revision_t convert_revision(v8::Isolate*                  isolate,
                                           const v8::Local<v8::Context>& context,
                                           const v8::Local<v8::Value>&   options,
                                           const char*                   key,
                                           svn_opt_revision_t            defaultValue) {
    if (options.IsEmpty())
        return defaultValue;

    if (!options->IsObject()) {
        throw svn::svn_type_error("");
    }

    auto object = options.As<v8::Object>();
    auto value  = object->Get(context, v8::New<v8::String>(isolate, key, v8::NewStringType::kInternalized));
    if (value.IsEmpty())
        return defaultValue;

    auto local = value.ToLocalChecked();
    if (local->IsUndefined())
        return defaultValue;

    if (local->IsNumber()) {
        auto simple = static_cast<svn_opt_revision_kind>(local->Int32Value());
        switch (simple) {
            case svn_opt_revision_unspecified:
            case svn_opt_revision_committed:
            case svn_opt_revision_previous:
            case svn_opt_revision_base:
            case svn_opt_revision_working:
            case svn_opt_revision_head:
                return svn_opt_revision_t{simple};
            case svn_opt_revision_number:
            case svn_opt_revision_date:
            default:
                throw svn::svn_type_error("");
        }
    }

    if (!local->IsObject())
        throw svn::svn_type_error("");

    object = local.As<v8::Object>();
    value  = object->Get(context, v8::New<v8::String>(isolate, "number", v8::NewStringType::kInternalized));
    if (!value.IsEmpty()) {
        local = value.ToLocalChecked();
        if (!local->IsNumber())
            throw svn::svn_type_error("");

        auto result         = svn_opt_revision_t{svn_opt_revision_number};
        result.value.number = local->Int32Value();
        return result;
    }

    value = object->Get(context, v8::New<v8::String>(isolate, "date", v8::NewStringType::kInternalized));
    if (!value.IsEmpty()) {
        local = value.ToLocalChecked();
        if (!local->IsNumber())
            throw svn::svn_type_error("");

        auto result       = svn_opt_revision_t{svn_opt_revision_date};
        result.value.date = local->IntegerValue();
        return result;
    }

    throw svn::svn_type_error("");
}

static svn::depth convert_depth(v8::Isolate*                  isolate,
                                const v8::Local<v8::Context>& context,
                                const v8::Local<v8::Value>&   options,
                                const char*                   key,
                                svn::depth                    defaultValue) {
    if (options.IsEmpty() || options->IsUndefined())
        return defaultValue;

    if (!options->IsObject())
        throw svn::svn_type_error("");

    auto object = options.As<v8::Object>();
    auto value  = object->Get(context, v8::New<v8::String>(isolate, key, v8::NewStringType::kInternalized)).ToLocalChecked();
    if (value->IsUndefined())
        return defaultValue;

    if (!value->IsNumber())
        throw svn::svn_type_error("");

    return static_cast<svn::depth>(value->Int32Value());
}

static void buffer_free_pointer(char*, void* hint) {
    delete static_cast<std::vector<char>*>(hint);
}

static v8::Local<v8::Object> buffer_from_vector(v8::Isolate* isolate, std::vector<char>& vector) {
    auto pointer = new std::vector<char>(std::move(vector));
    return node::Buffer::New(isolate, pointer->data(), pointer->size(), buffer_free_pointer, pointer).ToLocalChecked();
}

#define STRINGIFY_INTERNAL(X) #X
#define STRINGIFY(X) STRINGIFY_INTERNAL(X)

#define InternalizedString(value) \
    v8::New<v8::String>(isolate, value, v8::NewStringType::kInternalized, sizeof(value) - 1)

#define SetReadOnly(object, name, value)                  \
    (object)->DefineOwnProperty(context,                  \
                                InternalizedString(name), \
                                value,                    \
                                v8::PropertyAttributeEx::ReadOnlyDontDelete)

#define SetPrototypeMethod(signature, prototype, name, callback, length)                     \
    /* Add a scope to hide extra variables */                                                \
    {                                                                                        \
        auto function = v8::FunctionTemplate::New(isolate,                /* isolate */      \
                                                  callback,               /* callback */     \
                                                  v8::Local<v8::Value>(), /* data */         \
                                                  signature,              /* signature */    \
                                                  length);                /* length */       \
        function->RemovePrototype();                                                         \
        prototype->Set(InternalizedString(name), function, v8::PropertyAttribute::DontEnum); \
    }

namespace node {
void CLASS_NAME::init(v8::Local<v8::Object>   exports,
                      v8::Isolate*            isolate,
                      v8::Local<v8::Context>& context) {
    auto client    = v8::New<v8::FunctionTemplate>(isolate, create_instance);
    auto signature = v8::Signature::New(isolate, client);

    client->SetClassName(InternalizedString(EXPORT_NAME));
    client->ReadOnlyPrototype();

    client->InstanceTemplate()->SetInternalFieldCount(1);

    auto prototype = client->PrototypeTemplate();
    SetPrototypeMethod(signature, prototype, "add_to_changelist", add_to_changelist, 2);
    SetPrototypeMethod(signature, prototype, "get_changelists", get_changelists, 2);
    SetPrototypeMethod(signature, prototype, "remove_from_changelists", remove_from_changelists, 2);

    SetPrototypeMethod(signature, prototype, "add", add, 1);
    SetPrototypeMethod(signature, prototype, "cat", cat, 1);
    SetPrototypeMethod(signature, prototype, "checkout", checkout, 2);
    SetPrototypeMethod(signature, prototype, "commit", commit, 3);
    SetPrototypeMethod(signature, prototype, "info", info, 2);
    SetPrototypeMethod(signature, prototype, "remove", remove, 2);
    SetPrototypeMethod(signature, prototype, "revert", revert, 1);
    SetPrototypeMethod(signature, prototype, "status", status, 2);
    SetPrototypeMethod(signature, prototype, "update", update, 1);

    SetPrototypeMethod(signature, prototype, "get_working_copy_root", get_working_copy_root, 1);

    SetReadOnly(exports, EXPORT_NAME, client->GetFunction());
}

void CLASS_NAME::create_instance(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (!args.IsConstructCall()) {
        auto isolate = args.GetIsolate();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "Class constructor " STRINGIFY(CLASS_NAME) " cannot be invoked without 'new'")));
        return;
    }

    auto result = new CLASS_NAME();
    result->Wrap(args.This());
}

METHOD_BEGIN(add_to_changelist)
    auto raw_paths      = convert_array(args[0], false);
    auto raw_changelist = convert_string(args[1]);

    auto raw_depth = convert_depth(isolate, context, args[2], "depth", svn::depth::infinity);

    ASYNC_BEGIN(void, raw_paths, raw_changelist, raw_depth)
        _this->_client->add_to_changelist(raw_paths, raw_changelist, raw_depth);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate))
METHOD_END

METHOD_BEGIN(get_changelists)
    auto raw_path = convert_string(args[0]);

    if (!args[1]->IsFunction())
        throw svn::svn_type_error("");

    auto callback      = args[1].As<v8::Function>();
    auto _callback     = std::make_shared<v8::Global<v8::Function>>(isolate, callback);
    auto _raw_callback = [isolate, _callback](const char* path, const char* changelist) -> void {
        v8::HandleScope scope(isolate);

        const auto           argc       = 2;
        v8::Local<v8::Value> argv[argc] = {
            v8::New<v8::String>(isolate, path),
            v8::New<v8::String>(isolate, changelist)};

        auto callback = _callback->Get(isolate);
        callback->Call(v8::Undefined(isolate), argc, argv);
    };
    auto raw_callback = TO_ASYNC_CALLBACK(_raw_callback, const char*, const char*);

    auto raw_changelists = convert_array(args[2].As<v8::Object>()->Get(v8::New<v8::String>(isolate, "changelists")), true);
    auto raw_depth       = convert_depth(isolate, context, args[2], "depth", svn::depth::infinity);

    ASYNC_BEGIN(void, raw_path, raw_callback, raw_changelists, raw_depth)
        _this->_client->get_changelists(raw_path, raw_callback, raw_changelists, raw_depth);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(remove_from_changelists)
    auto raw_paths = convert_array(args[0], false);

    ASYNC_BEGIN(void, raw_paths)
        _this->_client->remove_from_changelists(raw_paths);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(add)
    auto raw_path = convert_string(args[0]);

    ASYNC_BEGIN(void, raw_path)
        _this->_client->add(raw_path);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(cat)
    auto raw_path = convert_string(args[0]);

    ASYNC_BEGIN(std::vector<char>, raw_path)
        ASYNC_RETURN(_this->_client->cat(raw_path));
    ASYNC_END()

    auto result = buffer_from_vector(isolate, ASYNC_RESULT);
    METHOD_RETURN(result);
METHOD_END

METHOD_BEGIN(checkout)
    auto raw_url  = convert_string(args[0]);
    auto raw_path = convert_string(args[1]);

    ASYNC_BEGIN(void, raw_url, raw_path)
        _this->_client->checkout(raw_url, raw_path);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

static svn::client::commit_callback convert_commit_callback(v8::Isolate* isolate, const v8::Local<v8::Value>& value) {
    if (!value->IsFunction())
        throw svn::svn_type_error("");

    auto callback      = value.As<v8::Function>();
    auto _callback     = std::make_shared<v8::Global<v8::Function>>(isolate, callback);
    auto _raw_callback = [isolate, _callback](const svn_commit_info_t* raw_info) -> void {
        v8::HandleScope scope(isolate);

        const auto argc = 1;
        auto       info = v8::New<v8::Object>(isolate);
        info->Set(InternalizedString("author"), v8::New<v8::String>(isolate, raw_info->author));
        info->Set(InternalizedString("date"), v8::New<v8::String>(isolate, raw_info->date));
        info->Set(InternalizedString("post_commit_err"), v8::New<v8::String>(isolate, raw_info->post_commit_err));
        info->Set(InternalizedString("repos_root"), v8::New<v8::String>(isolate, raw_info->repos_root));
        info->Set(InternalizedString("revision"), v8::New<v8::Integer>(isolate, raw_info->revision));
        v8::Local<v8::Value> argv[argc] = {info};

        auto callback = _callback->Get(isolate);
        callback->Call(v8::Undefined(isolate), argc, argv);
    };

    return TO_ASYNC_CALLBACK(_raw_callback, const svn_commit_info_t*);
}

METHOD_BEGIN(commit)
    auto raw_paths    = convert_array(args[0], false);
    auto raw_message  = convert_string(args[1]);
    auto raw_callback = convert_commit_callback(isolate, args[2]);

    ASYNC_BEGIN(void, raw_paths, raw_message, raw_callback)
        _this->_client->commit(raw_paths, raw_message, raw_callback);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(info)
    auto raw_path = convert_string(args[0]);

    v8::Local<v8::Value>    options;
    v8::Local<v8::Function> callback;
    if (args[1]->IsFunction()) {
        callback = args[1].As<v8::Function>();
    } else if (args[2]->IsFunction()) {
        options  = args[1];
        callback = args[2].As<v8::Function>();
    } else {
        throw svn::svn_type_error("");
    }

    auto _callback     = std::make_shared<v8::Global<v8::Function>>(isolate, callback);
    auto _raw_callback = [isolate, _callback](const char* path, const svn::info* raw_info) -> void {
        v8::HandleScope scope(isolate);

        auto info = v8::New<v8::Object>(isolate);
        info->Set(InternalizedString("path"), v8::New<v8::String>(isolate, path));
        info->Set(InternalizedString("last_changed_author"), v8::New<v8::String>(isolate, raw_info->last_changed_author));
        info->Set(InternalizedString("url"), v8::New<v8::String>(isolate, raw_info->URL));

        const auto           argc       = 1;
        v8::Local<v8::Value> argv[argc] = {info};

        auto callback = _callback->Get(isolate);
        callback->Call(v8::Undefined(isolate), argc, argv);
    };
    auto raw_callback = TO_ASYNC_CALLBACK(_raw_callback, const char*, const svn::info*);

    auto raw_peg_revision = convert_revision(isolate, context, options, "peg_revision", svn_opt_revision_t{svn_opt_revision_working});
    auto raw_revision     = convert_revision(isolate, context, options, "revision", svn_opt_revision_t{svn_opt_revision_working});
    auto raw_depth        = convert_depth(isolate, context, options, "depth", svn::depth::empty);

    ASYNC_BEGIN(void, raw_path, raw_callback, raw_peg_revision, raw_revision, raw_depth)
        _this->_client->info(raw_path, raw_callback, raw_peg_revision, raw_revision, raw_depth);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(remove)
    auto raw_paths    = convert_array(args[0], false);
    auto raw_callback = convert_commit_callback(isolate, args[1]);

    ASYNC_BEGIN(void, raw_paths, raw_callback)
        _this->_client->remove(raw_paths, raw_callback);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(revert)
    auto raw_paths = convert_array(args[0], false);

    ASYNC_BEGIN(void, raw_paths)
        _this->_client->revert(raw_paths);
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(status)
    auto raw_path = convert_string(args[0]);

    if (!args[1]->IsFunction())
        throw svn::svn_type_error("");

    auto callback      = args[1].As<v8::Function>();
    auto _callback     = std::make_shared<v8::Global<v8::Function>>(isolate, callback);
    auto _raw_callback = [isolate, _callback](const char* path, const svn::status* raw_info) -> void {
        v8::HandleScope scope(isolate);

        auto info = v8::New<v8::Object>(isolate);
        info->Set(InternalizedString("path"), v8::New<v8::String>(isolate, path));
        info->Set(InternalizedString("kind"), v8::New<v8::Integer>(isolate, static_cast<int32_t>(raw_info->kind)));
        info->Set(InternalizedString("node_status"), v8::New<v8::Integer>(isolate, static_cast<int32_t>(raw_info->node_status)));
        info->Set(InternalizedString("text_status"), v8::New<v8::Integer>(isolate, static_cast<int32_t>(raw_info->text_status)));
        info->Set(InternalizedString("prop_status"), v8::New<v8::Integer>(isolate, static_cast<int32_t>(raw_info->prop_status)));
        info->Set(InternalizedString("versioned"), v8::New<v8::Boolean>(isolate, raw_info->versioned));

        if (raw_info->changelist != nullptr)
            info->Set(InternalizedString("changelist"), v8::New<v8::String>(isolate, raw_info->changelist));

        const auto           argc       = 1;
        v8::Local<v8::Value> argv[argc] = {info};

        auto callback = _callback->Get(isolate);
        callback->Call(v8::Undefined(isolate), argc, argv);
    };
    auto raw_callback = TO_ASYNC_CALLBACK(_raw_callback, const char*, const svn::status*);

    ASYNC_BEGIN(void, raw_path, raw_callback)
        _this->_client->status(raw_path, std::move(raw_callback));
    ASYNC_END()

    ASYNC_RESULT;
    METHOD_RETURN(v8::Undefined(isolate));
METHOD_END

METHOD_BEGIN(update)
    auto raw_paths = convert_array(args[0], false);

    ASYNC_BEGIN(std::vector<svn_revnum_t>, raw_paths)
        ASYNC_RETURN(_this->_client->update(raw_paths));
    ASYNC_END(args)

    if (args[0]->IsString()) {
        auto result = v8::New<v8::Integer>(isolate, ASYNC_RESULT[0]);
        METHOD_RETURN(result);
    } else {
        auto vector = ASYNC_RESULT;
        auto result = v8::New<v8::Array>(isolate, static_cast<int32_t>(vector.size()));
        for (uint32_t i = 0; i < vector.size(); i++)
            result->Set(i, v8::New<v8::Integer>(isolate, vector[i]));
        METHOD_RETURN(result);
    }
METHOD_END

METHOD_BEGIN(get_working_copy_root)
    auto raw_path = convert_string(args[0]);

    ASYNC_BEGIN(std::string, raw_path)
        ASYNC_RETURN(_this->_client->get_working_copy_root(raw_path));
    ASYNC_END()

    auto result = v8::New<v8::String>(isolate, ASYNC_RESULT);
    METHOD_RETURN(result);
METHOD_END

CLASS_NAME::CLASS_NAME()
    : _client(new svn::client()) {}

CLASS_NAME::~CLASS_NAME() {}

} // namespace node
