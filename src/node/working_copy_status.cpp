#include "working_copy_status.hpp"

#include <svn_wc.h>

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

#define SET_WORKING_COPY_STATUS(name) SET_ENUM(object, svn_wc_status_, name)

namespace node {
namespace working_copy_status {
void init(v8::Local<v8::Object>   exports,
          v8::Isolate*            isolate,
          v8::Local<v8::Context>& context) {
    auto object = v8::New<v8::Object>(isolate);

    SET_WORKING_COPY_STATUS(none);
    SET_WORKING_COPY_STATUS(unversioned);
    SET_WORKING_COPY_STATUS(normal);
    SET_WORKING_COPY_STATUS(added);
    SET_WORKING_COPY_STATUS(missing);
    SET_WORKING_COPY_STATUS(deleted);
    SET_WORKING_COPY_STATUS(replaced);
    SET_WORKING_COPY_STATUS(modified);
    SET_WORKING_COPY_STATUS(conflicted);
    SET_WORKING_COPY_STATUS(ignored);
    SET_WORKING_COPY_STATUS(obstructed);
    SET_WORKING_COPY_STATUS(external);
    SET_WORKING_COPY_STATUS(incomplete);

    SetReadOnly(exports, "WorkingCopyStatus", object);
}
} // namespace working_copy_status
} // namespace node
