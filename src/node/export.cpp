#include <node.h>

#include <iostream>

#include <svn_client.h>
#include <svn_version.h>

#include <node/v8.hpp>

#include <node/node_client.hpp>

#include <node/enum/conflict_choose.hpp>
#include <node/enum/depth.hpp>
#include <node/enum/node_kind.hpp>
#include <node/enum/revision_kind.hpp>
#include <node/enum/status_kind.hpp>

#include <node/repos.hpp>

#include <objects/object.hpp>

using namespace std::literals;

namespace no {
void version(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();

    auto version = svn_client_version();

    no::object object(isolate);
    object["major"] = version->major;
    object["minor"] = version->minor;
    object["patch"] = version->patch;

    args.GetReturnValue().Set(object.value());
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
//         return iterator->yield(no::data(isolate, i));
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

void initialize(v8::Local<v8::Object> raw_exports) {
    no::object exports(raw_exports);

    raw_exports->SetAccessor(exports.context(),                                                                                            // context
                             no::name(exports.isolate(), "version"),                                                                       // name
                             version,                                                                                                      // getter
                             nullptr,                                                                                                      // setter
                             v8::MaybeLocal<v8::Value>(),                                                                                  // data
                             v8::AccessControl::ALL_CAN_READ,                                                                              // settings
                             static_cast<v8::PropertyAttribute>(no::property_attribute::read_only | no::property_attribute::dont_delete)); // attribute

    // NODE_SET_METHOD(exports, "test", test);

    client::initialize(exports);

    conflict_choose::initialize(exports);
    depth::initialize(exports);
    node_kind::initialize(exports);
    revision_kind::initialize(exports);
    status_kind::initialize(exports);
    //SvnError::Init(exports);

    repos::initialize(exports);
}

NODE_MODULE(svn, initialize)
} // namespace no
