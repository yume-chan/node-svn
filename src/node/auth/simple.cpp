#include <cstring>

#include "simple.hpp"

#define INTERNALIZED_STRING(value) \
    v8::New<v8::String>(isolate, value, v8::NewStringType::kInternalized, sizeof(value) - 1)

static std::string convert_string(const v8::Local<v8::Value>& value) {
    if (!value->IsString())
        throw svn::svn_type_error("");

    v8::String::Utf8Value utf8(value);
    auto                  length = static_cast<size_t>(utf8.length());

    if (std::strlen(*utf8) != length)
        throw svn::svn_type_error("");

    return std::string(*utf8, length);
}

using simple_auth_promise = std::promise<std::unique_ptr<svn::simple_auth>>;

static std::unique_ptr<svn::simple_auth> convert_simple_auth(v8::Isolate*                isolate,
                                                             const v8::Local<v8::Value>& value) {
    if (value->IsUndefined())
        return std::unique_ptr<svn::simple_auth>();

    if (value->IsObject()) {
        auto object = value.As<v8::Object>();
        try {
            auto username = convert_string(object->Get(INTERNALIZED_STRING("username")));
            auto password = convert_string(object->Get(INTERNALIZED_STRING("password")));
            auto may_save = object->Get(INTERNALIZED_STRING("may_save"))->BooleanValue();
            return std::make_unique<svn::simple_auth>(username, password, may_save);
        } catch (svn::svn_type_error&) {
            // TODO: add warning for wrong return value type.
            return std::unique_ptr<svn::simple_auth>();
        }
    }

    // TODO: add warning for wrong return value type.
    return std::unique_ptr<svn::simple_auth>();
}

static void simple_auth_then(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto            isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);

    auto context = isolate->GetCurrentContext();

    auto auth    = convert_simple_auth(isolate, args[0]);
    auto promise = static_cast<simple_auth_promise*>(args.Data().As<v8::External>()->Value());
    promise->set_value(std::move(auth));
}

namespace node {
simple_auth_provider::simple_auth_provider(v8::Isolate* isolate, v8::Local<v8::Function>& callback, bool is_async)
    : _isolate(isolate)
    , _callback(v8::Global<v8::Function>(isolate, callback))
    , _is_async(is_async) {
    if (is_async) {
        auto async = new invoke_function_async(_invoke_sync);
        _invoke    = async->bind();
    } else {
        _invoke = &_invoke_sync;
    }
}

simple_auth_provider::~simple_auth_provider() {
}

std::unique_ptr<svn::simple_auth> simple_auth_provider::operator()(const std::string& realm,
                                                                   const std::string& username,
                                                                   bool               may_save) {
    _promise = simple_auth_promise();
    _invoke(this, realm, username, may_save);
    return _promise.get_future().get();
}

void simple_auth_provider::_invoke_sync(simple_auth_provider* _this,
                                        const std::string&    realm,
                                        const std::string&    username,
                                        bool                  may_save) {
    auto            isolate = _this->_isolate;
    v8::HandleScope scope(isolate);
    auto            context = isolate->GetCurrentContext();

    const auto           argc       = 3;
    v8::Local<v8::Value> argv[argc] = {
        v8::New<v8::String>(isolate, realm.c_str(), v8::NewStringType::kNormal, realm.size()),
        v8::New<v8::String>(isolate, username.c_str(), v8::NewStringType::kNormal, username.size()),
        v8::New<v8::Boolean>(isolate, may_save)};

    auto callback = _this->_callback.Get(isolate);
    auto result   = callback->Call(context, v8::Undefined(isolate), argc, argv).ToLocalChecked();

    if (_this->_is_async && result->IsPromise()) {
        auto external = v8::New<v8::External>(isolate, &_this->_promise);
        auto then     = v8::Function::New(context, simple_auth_then, external).ToLocalChecked();
        result.As<v8::Promise>()->Then(context, then);
        return;
    }

    auto auth = convert_simple_auth(isolate, result);
    _this->_promise.set_value(std::move(auth));
}
} // namespace node
