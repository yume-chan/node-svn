#include "node_client.hpp"

#define CLASS_NAME client
#define EXPORT_NAME "Client"
#define ASYNC false

#define METHOD_BEGIN(name)                                               \
    void client::name(const v8::FunctionCallbackInfo<v8::Value>& args) { \
        auto isolate = args.GetIsolate();                                \
        auto context = isolate->GetCurrentContext();                     \
        try {                                                            \
            auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

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

#define METHOD_RETURN(value) \
    args.GetReturnValue().Set(value);

#define METHOD_END                                                                                          \
    }                                                                                                       \
    catch (svn::svn_type_error & error) {                                                                   \
        isolate->ThrowException(v8::Exception::TypeError(no::New(isolate, error.what()).As<v8::String>())); \
    }                                                                                                       \
    catch (svn::svn_error & raw_error) {                                                                    \
        auto error = copy_error(isolate, raw_error);                                                        \
        isolate->ThrowException(error);                                                                     \
    }                                                                                                       \
    }

#include "template/client.inc.cpp"
