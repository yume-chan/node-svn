#include "node_kind.hpp"

#include <cpp/types.hpp>

#define INTERNALIZED_STRING(value) \
    v8::New(isolate, value, v8::NewStringType::kInternalized, sizeof(value) - 1)

#define SET_ENUM(target, prefix, name)                                                                       \
    {                                                                                                        \
        auto key   = INTERNALIZED_STRING(#name);                                                             \
        auto value = static_cast<int32_t>(prefix::name);                                                     \
        target->DefineOwnProperty(context,                                                                   \
                                  key,                                                                       \
                                  v8::New(isolate, value),                                                   \
                                  v8::PropertyAttributeEx::ReadOnlyDontDelete);                              \
        target->DefineOwnProperty(context,                                                                   \
                                  v8::New(isolate, std::to_string(value), v8::NewStringType::kInternalized), \
                                  key,                                                                       \
                                  v8::PropertyAttributeEx::ReadOnlyDontDelete);                              \
    }

#define SetReadOnly(object, name, value)                   \
    (object)->DefineOwnProperty(context,                   \
                                INTERNALIZED_STRING(name), \
                                value,                     \
                                v8::PropertyAttributeEx::ReadOnlyDontDelete)

#define SET_NODE_KIND(name) SET_ENUM(object, svn::node_kind, name)

namespace node {
namespace node_kind {
void init(v8::Local<v8::Object>&  exports,
          v8::Isolate*            isolate,
          v8::Local<v8::Context>& context) {
    auto object = v8::New<v8::Object>(isolate);

    SET_NODE_KIND(none);
    SET_NODE_KIND(file);
    SET_NODE_KIND(dir);
    SET_NODE_KIND(unknown);

    SetReadOnly(exports, "NodeKind", object);
}
} // namespace node_kind
} // namespace node
