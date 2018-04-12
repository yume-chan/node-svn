#include "simple.hpp"

#include <cstring>
#include <functional>
#include <future>

#include <objects/object.hpp>

#include <uv/async.hpp>

namespace no {
namespace promise {
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
static std::future<T> then(v8::Isolate*                  isolate,
                           const v8::Local<v8::Promise>& promise,
                           ThenCallback<T>               then_callback,
                           ThenCallback<T>               catch_callback) {
    auto _promise = std::make_shared<std::promise<T>>();
    auto future   = _promise->get_future();

    auto context = isolate->GetEnteredContext();

    auto external = no::data(isolate, new then_data<T>{std::move(then_callback), _promise});
    auto function = v8::Function::New(context, invoke_callback<T>, external).ToLocalChecked();
    promise->Then(context, function).ToLocalChecked();

    external = no::data(isolate, new then_data<T>{std::move(catch_callback), _promise});
    function = v8::Function::New(context, invoke_callback<T>, external).ToLocalChecked();
    promise->Catch(context, function).ToLocalChecked();

    // switch (promise->State()) {
    //     case v8::Promise::kPending: {
    //         auto external = v8::External::New(isolate, new then_data<T>{std::move(then_callback), _promise});
    //         auto function = v8::Function::New(context, invoke_callback<T>, external).ToLocalChecked();
    //         promise->Then(context, function).ToLocalChecked();

    //         external = v8::External::New(isolate, new then_data<T>{std::move(catch_callback), _promise});
    //         function = v8::Function::New(context, invoke_callback<T>, external).ToLocalChecked();
    //         promise->Catch(context, function).ToLocalChecked();
    //     } break;
    //     case v8::Promise::kFulfilled:
    //         try {
    //             _promise->set_value(then_callback(isolate, promise->Result()));
    //         } catch (...) {
    //             _promise->set_exception(std::current_exception());
    //         }
    //         break;
    //     case v8::Promise::kRejected:
    //         try {
    //             _promise->set_value(catch_callback(isolate, promise->Result()));
    //         } catch (...) {
    //             _promise->set_exception(std::current_exception());
    //         }
    //         break;
    // }

    return future;
}
} // namespace promise
} // namespace no

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
        no::object object(value.As<v8::Object>());
        try {
            auto username = convert_string(object["username"]);
            auto password = convert_string(object["password"]);
            auto may_save = object["may_save"]->BooleanValue();
            return svn::simple_auth(std::move(username), std::move(password), may_save);
        } catch (const svn::svn_type_error&) {
            // TODO: add warning for wrong return value type.
            return {};
        }
    }

    // TODO: add warning for wrong return value type.
    return {};
}

namespace no {
using simple_auth_promise = std::promise<std::optional<svn::simple_auth>>;
using simple_auth_future  = std::future<std::optional<svn::simple_auth>>;

simple_auth_provider::simple_auth_provider(v8::Isolate* isolate)
    : _isolate(isolate)
    , _invoke(uv::make_async(&simple_auth_provider::_invoke_implement)) {
    v8::HandleScope scope(isolate);
    _functions.Reset(isolate, v8::Set::New(isolate));
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
    no::check_result(functions->Delete(context, function));
}

std::optional<svn::simple_auth> simple_auth_provider::operator()(const std::string&                      realm,
                                                                 const std::optional<const std::string>& username,
                                                                 bool                                    may_save) const {
    return _invoke(this, realm, username, may_save);
}

std::optional<svn::simple_auth> simple_auth_provider::_invoke_implement(const simple_auth_provider*             _this,
                                                                        const std::string&                      realm,
                                                                        const std::optional<const std::string>& username,
                                                                        bool                                    may_save) {
    auto            isolate = _this->_isolate;
    v8::HandleScope scope(isolate);
    auto            context   = isolate->GetEnteredContext();
    auto            undefined = v8::Undefined(isolate);

    const auto           argc       = 3;
    v8::Local<v8::Value> argv[argc] = {
        no::data(isolate, realm),
        no::data(isolate, username),
        no::data(isolate, may_save)};

    auto functions = _this->_functions.Get(isolate);
    auto array     = functions->AsArray();
    auto length    = array->Length();
    for (decltype(length) i = 0; i < length; i++) {
        auto function = array->Get(i).As<v8::Function>();
        auto value    = function->Call(context, undefined, argc, argv).ToLocalChecked();
        if (value->IsPromise()) {
            auto promise = value.As<v8::Promise>();
            auto result  = no::promise::then<std::optional<svn::simple_auth>>(isolate, promise, convert_simple_auth, convert_simple_auth).get();
            if (result) {
                return result;
            }
        }
    }

    return {};
}
} // namespace no
