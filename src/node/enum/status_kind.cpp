#include "status_kind.hpp"

#include "enum.hpp"

#define SET_STATUS_KIND(name) SET_ENUM(object, svn::status_kind, name)

namespace no {
namespace status_kind {
void init(v8::Local<v8::Object>&  exports,
          v8::Isolate*            isolate,
          v8::Local<v8::Context>& context) {
    auto object = no::New<v8::Object>(isolate);

    SET_STATUS_KIND(none);
    SET_STATUS_KIND(unversioned);
    SET_STATUS_KIND(normal);
    SET_STATUS_KIND(added);
    SET_STATUS_KIND(missing);
    SET_STATUS_KIND(deleted);
    SET_STATUS_KIND(replaced);
    SET_STATUS_KIND(modified);
    SET_STATUS_KIND(conflicted);
    SET_STATUS_KIND(ignored);
    SET_STATUS_KIND(obstructed);
    SET_STATUS_KIND(external);
    SET_STATUS_KIND(incomplete);

    SET_READONLY(exports, "StatusKind", object);
}
} // namespace status_kind
} // namespace no
