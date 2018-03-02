#include <cstring>
#include <functional>
#include <future>

#include <uv/async.hpp>

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
static std::future<T> Then(v8::Isolate*                  isolate,
                           const v8::Local<v8::Promise>& promise,
                           ThenCallback<T>               then_callback,
                           ThenCallback<T>               catch_callback) {
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

static std::optional<svn::simple_auth> convert_simple_auth(v8::Isolate*                isolate,
                                                           const v8::Local<v8::Value>& value) {
    if (value->IsUndefined())
        return {};

    if (value->IsObject()) {
        auto object = value.As<v8::Object>();
        try {
            auto username = convert_string(object->Get(INTERNALIZED_STRING("username")));
            auto password = convert_string(object->Get(INTERNALIZED_STRING("password")));
            auto may_save = object->Get(INTERNALIZED_STRING("may_save"))->BooleanValue();
            return svn::simple_auth(username, password, may_save);
        } catch (svn::svn_type_error&) {
            // TODO: add warning for wrong return value type.
            return {};
        }
    }

    // TODO: add warning for wrong return value type.
    return {};
}

namespace node {
using simple_auth_promise = std::promise<std::optional<svn::simple_auth>>;
using simple_auth_future  = std::future<std::optional<svn::simple_auth>>;

simple_auth_provider::simple_auth_provider(v8::Isolate* isolate, bool async)
    : _isolate(isolate)
    , _async(async) {
    v8::HandleScope scope(isolate);
    _functions.Reset(isolate, v8::Set::New(isolate));
}

simple_auth_provider::~simple_auth_provider() {
    _functions.Reset();
}

void simple_auth_provider::add(const v8::Local<v8::Function>& function) {
    v8::HandleScope scope(_isolate);

    auto context = _isolate->GetCurrentContext();

    auto functions = _functions.Get(_isolate);
    functions->Add(context, function).ToLocalChecked();
}

void simple_auth_provider::remove(const v8::Local<v8::Function>& function) {
    v8::HandleScope scope(_isolate);

    auto context = _isolate->GetCurrentContext();

    auto functions = _functions.Get(_isolate);
    functions->Delete(context, function);
}

std::optional<svn::simple_auth> simple_auth_provider::operator()(const std::string&                      realm,
                                                                 const std::optional<const std::string>& username,
                                                                 bool                                    may_save) {
                                                                     std::mem_fn(&node::simple_auth_provider::operator());
}

simple_auth_provider::operator svn::simple_auth_provider() {
    auto bind = std::bind(&node::simple_auth_provider::operator(),
                          this->shared_from_this(),
                          std::placeholders::_1,
                          std::placeholders::_2,
                          std::placeholders::_3);

    return std::make_shared<svn::simple_auth_provider::element_type>(bind);
}

// struct simple_auth_provider {
//   public:
//     simple_auth_provider(v8::Isolate* isolate, shared_callback callback, bool is_async)
//         : _isolate(isolate)
//         , _callback(callback)
//         , _is_async(is_async) {
//         if (is_async) {
//             auto async = new invoke_function_async(_invoke_sync);
//             _invoke    = async->bind();
//         } else {
//             _invoke = &_invoke_sync;
//         }
//     }

//     std::optional<svn::simple_auth> operator()(const std::string&                      realm,
//                                                const std::optional<const std::string>& username,
//                                                bool                                    may_save) {
//         auto future = _invoke(this, realm, username, may_save);
//         return future.get();
//     }

//   private:
//     static simple_auth_future _invoke_sync(simple_auth_provider*                   _this,
//                                            const std::string&                      realm,
//                                            const std::optional<const std::string>& username,
//                                            bool                                    may_save) {
//         auto            isolate = _this->_isolate;
//         v8::HandleScope scope(isolate);
//         auto            context = isolate->GetCurrentContext();

//         const auto           argc       = 3;
//         v8::Local<v8::Value> argv[argc] = {
//             v8::New(isolate, realm),
//             username ? v8::New(isolate, *username).As<v8::Value>() : v8::Undefined(isolate).As<v8::Value>(),
//             v8::New(isolate, may_save)};

//         auto callback = _this->_callback->Get(isolate);
//         auto result   = callback->Call(context, v8::Undefined(isolate), argc, argv).ToLocalChecked();

//         if (_this->_is_async && result->IsPromise()) {
//             return v8::PromiseEx::Then<std::optional<svn::simple_auth>>(isolate,
//                                                                         result.As<v8::Promise>(),
//                                                                         convert_simple_auth,
//                                                                         convert_simple_auth);
//         }

//         simple_auth_promise promise;
//         promise.set_value(convert_simple_auth(isolate, result));
//         return promise.get_future();
//     }

//     using invoke_function = std::function<simple_auth_future(simple_auth_provider*,
//                                                              const std::string&,
//                                                              const std::optional<const std::string>&,
//                                                              bool)>;

//     using invoke_function_async = uv::async<invoke_function,
//                                             simple_auth_future,
//                                             simple_auth_provider*,
//                                             const std::string&,
//                                             const std::optional<const std::string>&,
//                                             bool>;

//     v8::Isolate*    _isolate;
//     shared_callback _callback;
//     bool            _is_async;
//     invoke_function _invoke;
// };

// svn::simple_auth_provider make_simple_auth_provider(v8::Isolate* isolate, shared_callback callback, bool async) {
//     auto provider = std::make_shared<node::simple_auth_provider>(isolate, callback, async);

//     auto bind = std::bind(&node::simple_auth_provider::operator(),
//                           provider,
//                           std::placeholders::_1,
//                           std::placeholders::_2,
//                           std::placeholders::_3);

//     return std::make_shared<svn::simple_auth_provider::element_type>(bind);
// }
} // namespace node
