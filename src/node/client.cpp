#include "client.hpp"

#include <cpp\client.hpp>
#include <node\v8.hpp>
#include <node_object_wrap.h>

static std::string convert_string(const v8::Local<v8::Value>& value) {
    if (value.IsEmpty()) {
        auto isolate = v8::Isolate::GetCurrent();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "")));
    }

    if (!value->IsString()) {
        auto isolate = v8::Isolate::GetCurrent();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "")));
    }

    v8::String::Utf8Value utf8(value);
    auto                  length = utf8.length();

    if (strlen(*utf8) != length) {
        auto isolate = v8::Isolate::GetCurrent();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "")));
    }

    return std::string(*utf8, utf8.length());
}

static std::vector<std::string> convert_array(const v8::Local<v8::Value>& value, bool canBeNull) {
    if (value.IsEmpty()) {
        if (canBeNull)
            return std::vector<std::string>();

        auto isolate = v8::Isolate::GetCurrent();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "")));
        throw std::exception();
    }

    if (value->IsString()) {
        return std::vector<std::string>{convert_string(value)};
    }

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
}

static svn_depth_t convert_depth(const v8::Local<v8::Value>& value, svn_depth_t defaultValue) {
    if (value.IsEmpty())
        return defaultValue;

    if (!value->IsNumber()) {
        auto isolate = v8::Isolate::GetCurrent();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "")));
    }

    return static_cast<svn_depth_t>(value->Int32Value());
}

#define InternalizedString(value) v8::New<String>(isolate, value, NewStringType::kInternalized, sizeof(value) - 1)

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
    SetPrototypeMethod(signature, prototype, "addToChangelist", add_to_changelist, 2);
    SetPrototypeMethod(signature, prototype, "getChangelist", get_changelists, 2);
    SetPrototypeMethod(signature, prototype, "removeFromChangelists", remove_from_changelists, 2);
}

void client::create_instance(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

void client::add_to_changelist(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

    auto raw_paths      = convert_array(args[0], false);
    auto raw_changelist = convert_string(args[1]);

    _this->_client->add_to_changelist(raw_paths, raw_changelist);
}

void client::get_changelists(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

    auto raw_path = convert_string(args[0]);

    if (!args[1]->IsFunction()) {
        auto isolate = v8::Isolate::GetCurrent();
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, "")));
    }

    auto isolate      = args.GetIsolate();
    auto callback     = args[1].As<Function>();
    auto raw_callback = [isolate, &callback](const char* path, const char* changelist) -> void {
        const auto   argc       = 2;
        Local<Value> argv[argc] = {
            v8::New<String>(isolate, path),
            v8::New<String>(isolate, changelist)};

        callback->Call(v8::Undefined(isolate), argc, argv);
    };

    _this->_client->get_changelists(raw_path, raw_callback);
}

void client::remove_from_changelists(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

    auto raw_paths = convert_array(args[0], false);

    _this->_client->remove_from_changelists(raw_paths);
}

client::client()
    : _client() {}

client::~client() {}

} // namespace node
