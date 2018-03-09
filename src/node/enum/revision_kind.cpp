#include "revision_kind.hpp"

#include "enum.hpp"

#define SET_REVISION_KIND(name) SET_ENUM(object, svn::revision_kind, name)

namespace node {
namespace revision_kind {
void init(v8::Local<v8::Object>&  exports,
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
