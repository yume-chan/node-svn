#include "node_kind.hpp"

#include <svn_opt.h>

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

#define SET_REVISION_KIND(name) SET_ENUM(object, svn_opt_revision_, name)

namespace node {
namespace revision_kind {
void init(v8::Local<v8::Object>   exports,
          v8::Isolate*            isolate,
          v8::Local<v8::Context>& context) {
    auto object = v8::New<v8::Object>(isolate);

    SET_REVISION_KIND(unspecified);
    SET_REVISION_KIND(number);
    SET_REVISION_KIND(date);
    SET_REVISION_KIND(committed);
    SET_REVISION_KIND(previous);
    SET_REVISION_KIND(base);
    SET_REVISION_KIND(working);
    SET_REVISION_KIND(head);

    SetReadOnly(exports, "RevisionKind", object);
}
} // namespace revision_kind
} // namespace node