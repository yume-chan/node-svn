#include "async_client.hpp"

#include <uv/async.hpp>
#include <uv/work.hpp>

#define CLASS_NAME async_client
#define EXPORT_NAME "AsyncClient"
#define ASYNC true

// clang-format off

#define REPORT_ERROR                                                                                  \
    } catch (svn::svn_error& raw_error) {                                                             \
        auto _Error = copy_error(isolate, raw_error);                                                 \
        _Resolver->Reject(_Error);                                                                    \
    }

#define METHOD_BEGIN(name)                                                                     \
    v8::Local<v8::Value> async_client::name(const v8::FunctionCallbackInfo<v8::Value>& args) { \
        auto isolate = args.GetIsolate();                                                      \
        auto context = isolate->GetCurrentContext();                                           \
                                                                                               \
        auto _Resolver = no::New<v8::Promise::Resolver>(context);                              \
                                                                                               \
        try {

#define CONVERT_CALLBACK(callback) \
    uv::make_async(callback);

#define EXPAND(x) x

#ifdef __GNUC__
#define NUM_ARGS_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define NUM_ARGS(...) NUM_ARGS_IMPL(_, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#else
#define NUM_ARGS_COUNT(x0, x1, x2, x3, x4, x5, n, ...) n
#define NUM_ARGS_PAD(...) 0, __VA_ARGS__
#define NUM_ARGS_EXPAND(...) EXPAND(NUM_ARGS_COUNT(__VA_ARGS__, 5, 4, 3, 2, 1, 0))
#define NUM_ARGS(...) NUM_ARGS_EXPAND(NUM_ARGS_PAD(__VA_ARGS__))
#endif

#define CAPTURE_0()
#define CAPTURE_1(x) x = std::move(x),
#define CAPTURE_2(x, ...) CAPTURE_1(x) CAPTURE_1(__VA_ARGS__)
#define CAPTURE_3(x, ...) CAPTURE_1(x) EXPAND(CAPTURE_2(__VA_ARGS__))
#define CAPTURE_4(x, ...) CAPTURE_1(x) EXPAND(CAPTURE_3(__VA_ARGS__))
#define CAPTURE_5(x, ...) CAPTURE_1(x) EXPAND(CAPTURE_4(__VA_ARGS__))
#define CAPTURE_N(n, ...) EXPAND(CAPTURE_##n(__VA_ARGS__))
#define CAPTURE_EXPEND(n, ...) CAPTURE_N(n, __VA_ARGS__)
#define CAPTURE(...) CAPTURE_EXPEND(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

#define ASYNC_BEGIN(result, ...) \
    auto _Work = [CAPTURE(__VA_ARGS__) this]() -> result {

#define ASYNC_RETURN(value) \
    return value;

#define ASYNC_END(...)                                                                                                                      \
    };                                                                                                                                      \
                                                                                                                                            \
    v8::Global<v8::Promise::Resolver> __Resolver(isolate, _Resolver);                                                                       \
    auto _After_work = [CAPTURE(__VA_ARGS__) isolate, __Resolver = std::move(__Resolver)](std::future<decltype(_Work())> _Future) -> void { \
        v8::HandleScope _Scope(isolate);                                                                                                    \
		auto context = isolate->GetCurrentContext();                                                                                        \
                                                                                                                                            \
        auto _Resolver = __Resolver.Get(isolate);                                                                                           \
        try {                                                                                                                               \

#define ASYNC_RESULT \
    _Future.get()

#define METHOD_RETURN(value)                                      \
                _Resolver->Resolve(context, value);               \
            REPORT_ERROR;                                         \
        };                                                        \
                                                                  \
        uv::queue_work(std::move(_Work), std::move(_After_work)); \
    REPORT_ERROR;                                                 \
                                                                  \
    return _Resolver;                                             \
}

#include "template/client.inc.cpp"
