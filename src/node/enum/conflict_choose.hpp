#pragma once

#include "enum.hpp"

namespace no {
namespace conflict_choose {
void initialize(no::object& exports) {
    no::object object(exports.isolate());
    set_enum(object, "postpone", svn::conflict_choose::postpone);
    set_enum(object, "base", svn::conflict_choose::base);
    set_enum(object, "theirs_full", svn::conflict_choose::theirs_full);
    set_enum(object, "mine_full", svn::conflict_choose::mine_full);
    set_enum(object, "theirs_conflict", svn::conflict_choose::theirs_conflict);
    set_enum(object, "mine_conflict", svn::conflict_choose::mine_conflict);
    set_enum(object, "merged", svn::conflict_choose::merged);
    set_enum(object, "unspecified", svn::conflict_choose::unspecified);

    exports["ConflictChoose"].set(object, no::property_attribute::read_only);
}
} // namespace conflict_choose
} // namespace no
