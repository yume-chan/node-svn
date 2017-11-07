#include <node.h>

#include <svn_client.h>
#include <svn_version.h>

#include <node/node_client.hpp>
#include <node/v8.hpp>

#define ReadOnlyDontDelete (PropertyAttribute)(PropertyAttribute::ReadOnly | PropertyAttribute::DontDelete)

#define InternalizedString(value) v8::New<String>(isolate, value, NewStringType::kInternalized, sizeof(value) - 1)

#define SetReadOnly(object, name, value)                   \
    (object)->DefineOwnProperty(context,                   \
                                InternalizedString(#name), \
                                (value),                   \
                                ReadOnlyDontDelete)

namespace node {
void version(Local<Name> property, const PropertyCallbackInfo<Value>& args) {
    auto isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();

    auto version = svn_client_version();

    auto object = Object::New(isolate);
    SetReadOnly(object, major, v8::New<Integer>(isolate, version->major));
    SetReadOnly(object, minor, v8::New<Integer>(isolate, version->minor));
    SetReadOnly(object, patch, v8::New<Integer>(isolate, version->patch));
    args.GetReturnValue().Set(object);
}

// V8_METHOD_BEGIN(Test)
// {
// 	const char *c;
// 	vector<string> strings;

// 	{
// 		auto s = std::string("Hello world");
// 		c = s.c_str();
// 		strings.push_back(std::move(s));
// 	}

// 	auto len = strlen(c);
// }
// V8_METHOD_END;

void init(Local<Object> exports) {
    auto isolate = exports->GetIsolate();
    auto context = isolate->GetCurrentContext();

    exports->SetAccessor(context,                       // context
                         InternalizedString("version"), // name
                         version,                       // getter
                         nullptr,                       // setter
                         MaybeLocal<Value>(),           // data
                         AccessControl::ALL_CAN_READ,   // settings
                         ReadOnlyDontDelete);           // attribute

    // NODE_SET_METHOD(exports, "test", Test);

    client::init(exports, isolate, context);
    //SvnError::Init(exports, isolate, context);
}

NODE_MODULE(svn, init)
} // namespace node
