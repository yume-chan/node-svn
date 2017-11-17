#include "async_client.hpp"

#include <uv/async.hpp>
#include <uv/work.hpp>

#define CLASS_NAME async_client
#define EXPORT_NAME "AsyncClient"

#define METHOD_BEGIN(name)                                                     \
    void async_client::name(const v8::FunctionCallbackInfo<v8::Value>& args) { \
        auto isolate = args.GetIsolate();                                      \
        auto context = isolate->GetCurrentContext();                           \
                                                                               \
        auto resolver = v8::New<v8::Promise::Resolver>(context);               \
        args.GetReturnValue().Set(resolver);                                   \
                                                                               \
        auto _this = node::ObjectWrap::Unwrap<async_client>(args.Holder());

// TODO: Convert to async
#define TO_ASYNC_CALLBACK(callback, ...) \
    std::function<std::invoke_result_t<decltype(callback), __VA_ARGS__>(__VA_ARGS__)>(callback)

#define EXPAND(x) x

#define NUM_ARGS_COUNT(x0, x1, x2, x3, x4, x5, n, ...) n
#define NUM_ARGS_PAD(...) 0, __VA_ARGS__
#define NUM_ARGS_EXPAND(...) EXPAND(NUM_ARGS_COUNT(__VA_ARGS__, 5, 4, 3, 2, 1, 0))
#define NUM_ARGS(...) NUM_ARGS_EXPAND(NUM_ARGS_PAD(__VA_ARGS__))

// clang-format off

#define CAPTURE_0()
#define CAPTURE_1(x) x = std::move(x),
#define CAPTURE_2(x, ...) CAPTURE_1(x) CAPTURE_1(__VA_ARGS__)
#define CAPTURE_3(x, ...) CAPTURE_1(x) EXPAND(CAPTURE_2(__VA_ARGS__))
#define CAPTURE_4(x, ...) CAPTURE_1(x) EXPAND(CAPTURE_3(__VA_ARGS__))
#define CAPTURE_5(x, ...) CAPTURE_1(x) EXPAND(CAPTURE_4(__VA_ARGS__))
#define CAPTURE_N(n, ...) EXPAND(CAPTURE_##n(__VA_ARGS__))
#define CAPTURE_EXPEND(n, ...) CAPTURE_N(n, __VA_ARGS__)
#define CAPTURE(...) CAPTURE_EXPEND(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// clang-format on

#define ASYNC_BEGIN(result, ...) \
    auto do_work = [CAPTURE(__VA_ARGS__) _this]() -> result {

#define ASYNC_RETURN(value) \
    return value;

// clang-format off

#define ASYNC_END(...)                                                                                                                           \
    };                                                                                                                                           \
                                                                                                                                                 \
    v8::Global<v8::Promise::Resolver> _resolver(isolate, resolver);                                                                              \
    auto after_work = [CAPTURE(__VA_ARGS__) isolate, _resolver = std::move(_resolver)](std::future<decltype(do_work())> future) -> void { \
        v8::HandleScope scope(isolate);                                                                                                          \
                                                                                                                                                 \
        auto resolver = _resolver.Get(isolate);                                                                                                  \
        try {

// clang-format on

#define ASYNC_RESULT \
    future.get()

#define METHOD_RETURN(value) \
    resolver->Resolve(value);

// clang-format off

#define METHOD_END                                                                                  \
        }                                                                                           \
        catch (svn::svn_type_error & error) {                                                       \
            resolver->Reject(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what()))); \
        }                                                                                           \
        catch (svn::svn_error & error) {                                                            \
            resolver->Reject(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));     \
        }                                                                                           \
    };                                                                                              \
                                                                                                    \
    uv::queue_work(std::move(do_work), std::move(after_work));                                      \
}

// clang-format on

#include "client_template.hpp"
