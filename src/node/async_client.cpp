#include "async_client.hpp"

#include <future>
#include <uv.h>

#define CLASS_NAME async_client
#define EXPORT_NAME "AsyncClient"

#define METHOD_BEGIN(name)                                                             \
    void async_client::name(const v8::FunctionCallbackInfo<v8::Value>& args) {         \
        auto isolate = args.GetIsolate();                                              \
        auto context = isolate->GetCurrentContext();                                   \
                                                                                       \
        auto resolver  = v8::New<v8::Promise::Resolver>(context);                      \
        auto _resolver = new v8::Persistent<v8::Promise::Resolver>(isolate, resolver); \
                                                                                       \
        args.GetReturnValue().Set(resolver);                                           \
                                                                                       \
        auto _this = node::ObjectWrap::Unwrap<async_client>(args.Holder());

#define EXPAND(x) x

#define NUM_ARGS_COUNT(x0, x1, x2, x3, n, ...) n
#define NUM_ARGS_PAD(...) 0, __VA_ARGS__
#define NUM_ARGS_EXPAND(...) EXPAND(NUM_ARGS_COUNT(__VA_ARGS__, 3, 2, 1, 0))
#define NUM_ARGS(...) NUM_ARGS_EXPAND(NUM_ARGS_PAD(__VA_ARGS__))

#define CAPTURE_0() _this
#define CAPTURE_1(x) x = std::move(x), _this
#define CAPTURE_2(x, ...) x = std::move(x), CAPTURE_1(__VA_ARGS__)
#define CAPTURE_3(x, ...) x = std::move(x), EXPAND(CAPTURE_2(__VA_ARGS__))
#define CAPTURE_N(n, ...) EXPAND(CAPTURE_##n(__VA_ARGS__))
#define CAPTURE_EXPEND(n, ...) CAPTURE_N(n, __VA_ARGS__)
#define CAPTURE(...) CAPTURE_EXPEND(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

#define ASYNC_BEGIN(result, ...) \
    auto work = [CAPTURE(__VA_ARGS__)]() -> result {

#define ASYNC_VOID(value) \
    value;

#define ASYNC_RETURN(value) \
    return value;

#define ASYNC_END \
    }             \
    ;             \
                  \
    auto after_work = [isolate, args, _resolver](std::future<decltype(work())> future) -> void {\
        v8::HandleScope scope(isolate);\
        auto resolver = _resolver->Get(isolate);\
        try {

#define ASYNC_RESULT \
    future.get()

#define METHOD_RETURN(value) \
    resolver->Resolve(value);

template <class T>
struct data_t {
    explicit data_t(std::function<T(void)>&&              work,
                    std::function<void(std::future<T>)>&& after_work)
        : work(work)
        , after_work(after_work)
        , promise() {}

    const std::function<T(void)>              work;
    const std::function<void(std::future<T>)> after_work;

    std::promise<T> promise;
};

template <class T>
static void queue_work(std::function<T(void)> work, std::function<void(std::future<T>)> after_work) {
    auto uv_work  = new uv_work_t();
    auto data     = new data_t<T>(std::move(work), std::move(after_work));
    uv_work->data = data;

    auto invoke_work = [](uv_work_t* req) -> void {
        auto data = static_cast<data_t<T>*>(req->data);
        try {
            auto result = data->work();
            data->promise.set_value(result);
        } catch (...) {
            data->promise.set_exception(std::current_exception());
        }
    };

    auto invoke_after_work = [](uv_work_t* req, int status) -> void {
        auto data   = static_cast<data_t<T>*>(req->data);
        auto future = data->promise.get_future();
        data->after_work(std::move(future));

        delete data;
        delete static_cast<uv_work_t*>(req);
    };

    uv_queue_work(uv_default_loop(), uv_work, invoke_work, invoke_after_work);
}

template <>
static void queue_work<void>(std::function<void(void)> work, std::function<void(std::future<void>)> after_work) {
    auto uv_work  = new uv_work_t();
    uv_work->data = new data_t<void>(std::move(work), std::move(after_work));

    auto invoke_work = [](uv_work_t* req) -> void {
        auto data = static_cast<data_t<void>*>(req->data);
        try {
            data->work();
            data->promise.set_value();
        } catch (...) {
            data->promise.set_exception(std::current_exception());
        }
    };

    auto invoke_after_work = [](uv_work_t* req, int status) -> void {
        auto data = static_cast<data_t<void>*>(req->data);
        data->after_work(data->promise.get_future());

        delete data;
        delete static_cast<uv_work_t*>(req);
    };

    uv_queue_work(uv_default_loop(), uv_work, invoke_work, invoke_after_work);
}

#define METHOD_END                                                                              \
    }                                                                                           \
    catch (svn::svn_type_error & error) {                                                       \
        resolver->Reject(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what()))); \
    }                                                                                           \
    catch (svn::svn_error & error) {                                                            \
        resolver->Reject(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));     \
    }                                                                                           \
    }                                                                                           \
    ;                                                                                           \
                                                                                                \
    queue_work<decltype(work())>(work, after_work);                                             \
    }

#include "client_template.hpp"
