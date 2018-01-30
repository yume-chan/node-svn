#include "conflict_choose.hpp"

#include <cpp/types.hpp>

#define InternalizedString(value) \
    v8::New<v8::String>(isolate, value, v8::NewStringType::kInternalized, sizeof(value) - 1)

#define SET_ENUM(target, prefix, name)                                                                                   \
    {                                                                                                                    \
        auto key   = InternalizedString(#name);                                                                          \
        auto value = static_cast<int32_t>(prefix::name);                                                                 \
        target->DefineOwnProperty(context,                                                                               \
                                  key,                                                                                   \
                                  v8::New<v8::Integer>(isolate, value),                                                  \
                                  v8::PropertyAttributeEx::ReadOnlyDontDelete);                                          \
        target->DefineOwnProperty(context,                                                                               \
                                  v8::New<v8::String>(isolate, std::to_string(value), v8::NewStringType::kInternalized), \
                                  key,                                                                                   \
                                  v8::PropertyAttributeEx::ReadOnlyDontDelete);                                          \
    }

#define SetReadOnly(object, name, value)                  \
    (object)->DefineOwnProperty(context,                  \
                                InternalizedString(name), \
                                value,                    \
                                v8::PropertyAttributeEx::ReadOnlyDontDelete)

#define SET_CONFLICT_CHOOSE(name) SET_ENUM(object, svn::conflict_choose, name)

namespace node {
namespace conflict_choose {
void init(v8::Local<v8::Object>   exports,
          v8::Isolate*            isolate,
          v8::Local<v8::Context>& context) {
    auto object = v8::New<v8::Object>(isolate);

    SET_CONFLICT_CHOOSE(postpone);
    SET_CONFLICT_CHOOSE(base);
    SET_CONFLICT_CHOOSE(theirs_full);
    SET_CONFLICT_CHOOSE(mine_full);
    SET_CONFLICT_CHOOSE(theirs_conflict);
    SET_CONFLICT_CHOOSE(mine_conflict);
    SET_CONFLICT_CHOOSE(merged);
    SET_CONFLICT_CHOOSE(unspecified);

    SetReadOnly(exports, "ConflictChoose", object);
}
} // namespace status_kind
} // namespace node
