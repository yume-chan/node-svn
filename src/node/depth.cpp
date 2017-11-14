#include "depth.hpp"

#include <svn_types.h>

#define InternalizedString(value) \
    v8::New<v8::String>(isolate, value, v8::NewStringType::kInternalized, sizeof(value) - 1)

#define SET_ENUM(target, prefix, name)                                                                                          \
    {                                                                                                                           \
        auto key = InternalizedString(#name);                                                                                   \
        target->DefineOwnProperty(context,                                                                                      \
                                  key,                                                                                          \
                                  v8::New<v8::Integer>(isolate, prefix##name),                                                  \
                                  v8::PropertyAttributeEx::ReadOnlyDontDelete);                                                 \
        target->DefineOwnProperty(context,                                                                                      \
                                  v8::New<v8::String>(isolate, std::to_string(prefix##name), v8::NewStringType::kInternalized), \
                                  key,                                                                                          \
                                  v8::PropertyAttributeEx::ReadOnlyDontDelete);                                                 \
    }

#define SetReadOnly(object, name, value)                  \
    (object)->DefineOwnProperty(context,                  \
                                InternalizedString(name), \
                                value,                    \
                                v8::PropertyAttributeEx::ReadOnlyDontDelete)

#define SetDepth(name) SET_ENUM(object, svn_depth_, name)

namespace node {
namespace depth {
void init(v8::Local<v8::Object>   exports,
          v8::Isolate*            isolate,
          v8::Local<v8::Context>& context) {
    auto object = v8::New<v8::Object>(isolate);

    SetDepth(unknown);
    SetDepth(empty);
    SetDepth(files);
    SetDepth(immediates);
    SetDepth(infinity);

    SetReadOnly(exports, "Depth", object);
}
} // namespace depth
} // namespace node
