#include "node_client.hpp"

#include <node_buffer.h>
#include <node_object_wrap.h>

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

static std::vector<std::string> convert_array(const v8::Local<v8::Value>& value, bool canBeNull) {
    if (value.IsEmpty()) {
        if (canBeNull)
            return std::vector<std::string>();

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

static svn_depth_t convert_depth(const v8::Local<v8::Value>& value, svn_depth_t defaultValue) {
    if (value.IsEmpty())
        return defaultValue;

    if (!value->IsNumber())
        throw svn::svn_type_error("");

    return static_cast<svn_depth_t>(value->Int32Value());
}

#define ReadOnlyDontDelete static_cast<PropertyAttribute>(PropertyAttribute::ReadOnly | PropertyAttribute::DontDelete)

#define InternalizedString(value) v8::New<String>(isolate, value, NewStringType::kInternalized, sizeof(value) - 1)

#define SetReadOnly(object, name, value)                  \
    (object)->DefineOwnProperty(context,                  \
                                InternalizedString(name), \
                                value,                    \
                                ReadOnlyDontDelete)

#define SetPrototypeMethod(signature, prototype, name, callback, length)                  \
    /* Add a scope to hide extra variables */                                             \
    {                                                                                     \
        auto function = v8::FunctionTemplate::New(isolate,                /* isolate */   \
                                                  callback,               /* callback */  \
                                                  v8::Local<v8::Value>(), /* data */      \
                                                  signature,              /* signature */ \
                                                  length);                /* length */    \
        function->RemovePrototype();                                                      \
        prototype->Set(InternalizedString(name), function, PropertyAttribute::DontEnum);  \
    }

namespace node {
void client::init(v8::Local<v8::Object> exports, v8::Isolate* isolate, v8::Local<v8::Context>& context) {
    auto client    = v8::New<v8::FunctionTemplate>(isolate, create_instance);
    auto signature = v8::Signature::New(isolate, client);

    client->SetClassName(InternalizedString("Client"));
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

    SetReadOnly(exports, "Client", client->GetFunction());
}

void client::create_instance(const v8::FunctionCallbackInfo<v8::Value>& args) {
    if (!args.IsConstructCall()) {
        auto isolate = args.GetIsolate();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "Class constructor Client cannot be invoked without 'new'")));
        return;
    }

    auto result = new client();
    result->Wrap(args.This());
}

void client::add_to_changelist(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_paths      = convert_array(args[0], false);
        auto raw_changelist = convert_string(args[1]);

        _this->_client->add_to_changelist(raw_paths, raw_changelist);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

void client::get_changelists(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_path = convert_string(args[0]);

        if (!args[1]->IsFunction())
            throw svn::svn_type_error("");

        auto callback     = args[1].As<Function>();
        auto raw_callback = [isolate, &callback](const char* path, const char* changelist) -> void {
            const auto   argc       = 2;
            Local<Value> argv[argc] = {
                v8::New<String>(isolate, path),
                v8::New<String>(isolate, changelist)};

            callback->Call(v8::Undefined(isolate), argc, argv);
        };

        _this->_client->get_changelists(raw_path, raw_callback);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

void client::remove_from_changelists(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_paths = convert_array(args[0], false);

        _this->_client->remove_from_changelists(raw_paths);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

void client::add(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_path = convert_string(args[0]);

        _this->_client->add(raw_path);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

void free_cat_buffer(char*, void* hint) {
    delete static_cast<std::vector<char>*>(hint);
}

void client::cat(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_path = convert_string(args[0]);

        auto buffer   = new std::vector<char>();
        auto callback = [buffer](const char* data, size_t length) -> void {
            auto end = data + length;
            buffer->insert(buffer->end(), data, end);
        };

        _this->_client->cat(raw_path, callback);

        args.GetReturnValue().Set(node::Buffer::New(isolate, buffer->data(), buffer->size(), free_cat_buffer, buffer).ToLocalChecked());
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

void client::checkout(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_url  = convert_string(args[0]);
        auto raw_path = convert_string(args[1]);

        _this->_client->checkout(raw_url, raw_path);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

svn::client::commit_callback invoke_commit_callback(v8::Isolate* isolate, const v8::Local<v8::Value>& value) {
    if (!value->IsFunction())
        throw svn::svn_type_error("");
    auto callback = value.As<Function>();

    return [isolate, &callback](const svn_commit_info_t* raw_info) -> void {
        const auto argc = 1;
        auto       info = v8::New<Object>(isolate);
        info->Set(InternalizedString("author"), v8::New<v8::String>(isolate, raw_info->author));
        info->Set(InternalizedString("date"), v8::New<v8::String>(isolate, raw_info->date));
        info->Set(InternalizedString("post_commit_err"), v8::New<v8::String>(isolate, raw_info->post_commit_err));
        info->Set(InternalizedString("repos_root"), v8::New<v8::String>(isolate, raw_info->repos_root));
        info->Set(InternalizedString("revision"), v8::New<v8::Integer>(isolate, raw_info->revision));
        Local<Value> argv[argc] = {info};

        callback->Call(v8::Undefined(isolate), argc, argv);
    };
}

void client::commit(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_paths    = convert_array(args[0], false);
        auto raw_message  = convert_string(args[1]);
        auto raw_callback = invoke_commit_callback(isolate, args[2]);

        _this->_client->commit(raw_paths, raw_message, raw_callback);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

void client::info(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_path = convert_string(args[0]);

        if (!args[1]->IsFunction())
            throw svn::svn_type_error("");

        auto callback     = args[1].As<Function>();
        auto raw_callback = [isolate, &callback](const char* path, const svn::client_info* raw_info) -> void {
            const auto argc = 2;
            auto       info = v8::New<Object>(isolate);
            info->Set(InternalizedString("last_changed_author"), v8::New<v8::String>(isolate, raw_info->last_changed_author));
            info->Set(InternalizedString("url"), v8::New<v8::String>(isolate, raw_info->URL));
            Local<Value> argv[argc] = {
                v8::New<v8::String>(isolate, path),
                info};

            callback->Call(v8::Undefined(isolate), argc, argv);
        };

        _this->_client->info(raw_path, raw_callback);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

void client::remove(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_paths    = convert_array(args[0], false);
        auto raw_callback = invoke_commit_callback(isolate, args[1]);

        _this->_client->remove(raw_paths, raw_callback);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

void client::revert(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_paths = convert_array(args[0], false);

        _this->_client->revert(raw_paths);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

void client::status(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_path = convert_string(args[0]);

        if (!args[1]->IsFunction())
            throw svn::svn_type_error("");

        auto callback     = args[1].As<Function>();
        auto raw_callback = [isolate, &callback](const char* path, const svn_client_status_t* raw_info) -> void {
            const auto   argc       = 2;
            auto         info       = v8::New<Object>(isolate);
            Local<Value> argv[argc] = {
                v8::New<v8::String>(isolate, path),
                info};

            callback->Call(v8::Undefined(isolate), argc, argv);
        };

        _this->_client->status(raw_path,
                               raw_callback);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

void client::update(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_path = convert_string(args[0]);

        auto raw_result = _this->_client->update(raw_path);
        auto result     = v8::New<Integer>(isolate, raw_result);
        args.GetReturnValue().Set(result);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

void client::get_working_copy_root(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    try {
        auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

        auto raw_path = convert_string(args[0]);

        auto raw_result = _this->_client->get_working_copy_root(raw_path);
        auto result     = v8::New<String>(isolate, raw_result);
        args.GetReturnValue().Set(result);
    } catch (svn::svn_type_error& error) {
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what())));
    } catch (svn::svn_error& error) {
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));
    }
}

client::client()
    : _client(new svn::client()) {}

client::~client() {}

} // namespace node
