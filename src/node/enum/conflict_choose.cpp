#include "conflict_choose.hpp"

#include "enum.hpp"

#define SET_CONFLICT_CHOOSE(name) SET_ENUM(object, svn::conflict_choose, name)

namespace node {
namespace conflict_choose {
void init(v8::Local<v8::Object>&  exports,
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
} // namespace conflict_choose
} // namespace node
