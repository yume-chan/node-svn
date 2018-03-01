#include <node.h>

#include <svn_client.h>
#include <svn_version.h>

#include <node/v8.hpp>

#include <node/async_client.hpp>
#include <node/node_client.hpp>

#include <node/enum/conflict_choose.hpp>
#include <node/enum/depth.hpp>
#include <node/enum/node_kind.hpp>
#include <node/enum/revision_kind.hpp>
#include <node/enum/status_kind.hpp>

#define INTERNALIZED_STRING(value) v8::New(isolate, value, v8::NewStringType::kInternalized, sizeof(value) - 1)

#define SetReadOnly(object, name, value)                  \
    (object)->DefineOwnProperty(context,                  \
                                INTERNALIZED_STRING(name), \
                                (value),                  \
                                v8::PropertyAttributeEx::ReadOnlyDontDelete)

namespace node {
void version(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();

    auto version = svn_client_version();

    auto object = v8::Object::New(isolate);
    SetReadOnly(object, "major", v8::New(isolate, version->major));
    SetReadOnly(object, "minor", v8::New(isolate, version->minor));
    SetReadOnly(object, "patch", v8::New(isolate, version->patch));
    args.GetReturnValue().Set(object);
}

static void Test(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();

    auto callback = args[0].As<v8::Function>();
	callback->Call(callback->CreationContext(), v8::Undefined(isolate), 0, nullptr);
}

#include <type_traits>

void init(v8::Local<v8::Object> exports) {
    auto isolate = exports->GetIsolate();
    auto context = isolate->GetCurrentContext();

    exports->SetAccessor(context,                                      // context
                         INTERNALIZED_STRING("version"),                // name
                         version,                                      // getter
                         nullptr,                                      // setter
                         v8::MaybeLocal<v8::Value>(),                  // data
                         v8::AccessControl::ALL_CAN_READ,              // settings
                         v8::PropertyAttributeEx::ReadOnlyDontDelete); // attribute

    NODE_SET_METHOD(exports, "test", Test);

    async_client::init(exports, isolate, context);
    client::init(exports, isolate, context);

    conflict_choose::init(exports, isolate, context);
    depth::init(exports, isolate, context);
    node_kind::init(exports, isolate, context);
    revision_kind::init(exports, isolate, context);
    status_kind::init(exports, isolate, context);
    //SvnError::Init(exports, isolate, context);
}

NODE_MODULE(svn, init)
} // namespace node
