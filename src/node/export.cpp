#include <node.h>

#include <iostream>

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

using namespace std::literals;

#define SetReadOnly(object, name, value)                                                  \
    (object)->DefineOwnProperty(context,                                                  \
                                no::New(isolate, name, v8::NewStringType::kInternalized), \
                                (value),                                                  \
                                no::PropertyAttribute::ReadOnlyDontDelete)

namespace no {
void version(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();

    auto version = svn_client_version();

    auto object = v8::Object::New(isolate);
    SetReadOnly(object, "major", no::New(isolate, version->major));
    SetReadOnly(object, "minor", no::New(isolate, version->minor));
    SetReadOnly(object, "patch", no::New(isolate, version->patch));
    args.GetReturnValue().Set(object);
}

// #include <node/iterator.hpp>
// #include <uv/work.hpp>

// static void test(const v8::FunctionCallbackInfo<v8::Value>& args) {
//     auto isolate = args.GetIsolate();
//     auto context = isolate->GetCurrentContext();

//     auto iterator = std::make_shared<no::async_iterator>(isolate, context);
//     args.GetReturnValue().Set(iterator->get());

//     auto async = [isolate, iterator](int32_t i) -> uv::future<void> {
//         v8::HandleScope scope(isolate);
//         return iterator->yield(no::New(isolate, i));
//     };

//     auto work = [async = uv::make_async(async)]() -> void {
//         for (auto i = 0; i < 5; i++) {
//             async(i);
//         }
//     };

//     auto after_work = [isolate, iterator](std::future<void>) -> void {
//         iterator->end();
//     };

//     uv::queue_work(work, after_work);
// }

void init(v8::Local<v8::Object> exports) {
    auto isolate = exports->GetIsolate();
    auto context = isolate->GetCurrentContext();

    exports->SetAccessor(context,                                                       // context
                         no::New(isolate, "version", v8::NewStringType::kInternalized), // name
                         version,                                                       // getter
                         nullptr,                                                       // setter
                         v8::MaybeLocal<v8::Value>(),                                   // data
                         v8::AccessControl::ALL_CAN_READ,                               // settings
                         no::PropertyAttribute::ReadOnlyDontDelete);                    // attribute

    // NODE_SET_METHOD(exports, "test", test);

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
} // namespace no
