#include "node_client.hpp"

#define CLASS_NAME client
#define EXPORT_NAME "Client"
#define ASYNC false

#define METHOD_BEGIN(name)                                                               \
    v8::Local<v8::Value> client::name(const v8::FunctionCallbackInfo<v8::Value>& args) { \
        auto isolate = args.GetIsolate();                                                \
        auto context = isolate->GetCurrentContext();                                     \
        try {

#define CONVERT_CALLBACK(callback) \
    callback

template <class T>
class future {
  public:
    future() {}

    T value;

    T get() {
        return value;
    }
};

template <>
class future<void> {
  public:
    future() {}

    void get() {}
};

#define ASYNC_BEGIN(result, ...) \
    future<result> future;

#define ASYNC_RETURN(result) \
    future.value = result;

#define ASYNC_END(...)

#define ASYNC_RESULT \
    future.get()

// clang-format off

#define METHOD_RETURN(value)                             \
            return value;                                \
        } catch (svn::svn_error & raw_error) {           \
            auto error = copy_error(isolate, raw_error); \
            isolate->ThrowException(error);              \
            return v8::Local<v8::Value>();               \
        }                                                \
    }

// clang-format on

#include "template/client.inc.cpp"
