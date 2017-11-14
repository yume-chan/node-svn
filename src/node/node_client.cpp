#include "node_client.hpp"

#define CLASS_NAME client
#define EXPORT_NAME "Client"

#define METHOD_BEGIN(name)                                               \
    void client::name(const v8::FunctionCallbackInfo<v8::Value>& args) { \
        auto isolate = args.GetIsolate();                                \
        try {                                                            \
            auto _this = node::ObjectWrap::Unwrap<client>(args.Holder());

#define ASYNC_BEGIN(...)

#define ASYNC_VOID(value) \
    value;                \
    auto sync_result = 1;

#define ASYNC_RETURN(value) \
    auto sync_result = value;

#define ASYNC_END

#define ASYNC_RESULT \
    sync_result

#define METHOD_RETURN(value) \
    args.GetReturnValue().Set(value);

#define METHOD_END                                                                                     \
    }                                                                                                  \
    catch (svn::svn_type_error & error) {                                                              \
        isolate->ThrowException(v8::Exception::TypeError(v8::New<v8::String>(isolate, error.what()))); \
    }                                                                                                  \
    catch (svn::svn_error & error) {                                                                   \
        isolate->ThrowException(v8::Exception::Error(v8::New<v8::String>(isolate, error.what())));     \
    }                                                                                                  \
    }

#include "client_template.hpp"
