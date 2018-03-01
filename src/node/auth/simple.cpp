#include <cstring>

#include "simple.hpp"

#define INTERNALIZED_STRING(value) \
    v8::New(isolate, value, v8::NewStringType::kInternalized, sizeof(value) - 1)

namespace v8 {
namespace PromiseEx {
template <class T>
using ThenCallback = std::function<T(v8::Isolate*, const v8::Local<v8::Value>& value)>;

template <class T>
struct then_data {
    ThenCallback<T>                  callback;
    std::shared_ptr<std::promise<T>> promise;
};

template <class T>
static void invoke_callback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    auto data    = static_cast<then_data<T>*>(args.Data().As<v8::External>()->Value());
    try {
        data->promise->set_value(data->callback(isolate, args[0]));
    } catch (...) {
        data->promise->set_exception(std::current_exception());
    }

    delete data;
}

template <class T>
static std::future<T> Then(v8::Isolate* isolate, const v8::Local<v8::Promise>& promise, ThenCallback<T> then_callback, ThenCallback<T> catch_callback) {
    auto _promise = std::make_shared<std::promise<T>>();
    auto future   = _promise->get_future();

    auto context = isolate->GetCurrentContext();

    switch (promise->State()) {
        case v8::Promise::kPending: {
            auto external = v8::External::New(isolate, new then_data<T>{std::move(then_callback), _promise});
            auto function = v8::Function::New(context, invoke_callback<T>, external).ToLocalChecked();
            promise->Then(context, function).ToLocalChecked();

            external = v8::External::New(isolate, new then_data<T>{std::move(catch_callback), _promise});
            function = v8::Function::New(context, invoke_callback<T>, external).ToLocalChecked();
            promise->Catch(context, function).ToLocalChecked();
        } break;
        case v8::Promise::kFulfilled:
            try {
                _promise->set_value(then_callback(isolate, promise->Result()));
            } catch (...) {
                _promise->set_exception(std::current_exception());
            }
            break;
        case v8::Promise::kRejected:
            try {
                _promise->set_value(catch_callback(isolate, promise->Result()));
            } catch (...) {
                _promise->set_exception(std::current_exception());
            }
            break;
    }

    return future;
}
} // namespace PromiseEx
} // namespace v8

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

namespace node {
simple_auth_provider::simple_auth_provider(v8::Isolate*             isolate,
                                           v8::Local<v8::Function>& callback,
                                           bool                     is_async)
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
    auto future = _invoke(this, realm, username, may_save);
    return future.get();
}

simple_auth_future simple_auth_provider::_invoke_sync(simple_auth_provider* _this,
                                                      const std::string&    realm,
                                                      const std::string&    username,
                                                      bool                  may_save) {
    auto            isolate = _this->_isolate;
    v8::HandleScope scope(isolate);
    auto            context = isolate->GetCurrentContext();

    const auto           argc       = 3;
    v8::Local<v8::Value> argv[argc] = {
        v8::New(isolate, realm),
        v8::New(isolate, username),
        v8::New(isolate, may_save)};

    auto callback = _this->_callback.Get(isolate);
    auto result   = callback->Call(context, v8::Undefined(isolate), argc, argv).ToLocalChecked();

    if (_this->_is_async && result->IsPromise()) {
        return v8::PromiseEx::Then<std::unique_ptr<svn::simple_auth>>(isolate, result.As<v8::Promise>(), convert_simple_auth, convert_simple_auth);
    }

    simple_auth_promise promise;
    promise.set_value(convert_simple_auth(isolate, result));
    return promise.get_future();
}
} // namespace node
