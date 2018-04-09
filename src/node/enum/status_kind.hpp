#pragma once

#include "enum.hpp"

namespace no {
namespace status_kind {
void initialize(no::object& exports) {
    no::object object(exports.isolate());
    set_enum(object, "none", svn::status_kind::none);
    set_enum(object, "unversioned", svn::status_kind::unversioned);
    set_enum(object, "normal", svn::status_kind::normal);
    set_enum(object, "added", svn::status_kind::added);
    set_enum(object, "missing", svn::status_kind::missing);
    set_enum(object, "deleted", svn::status_kind::deleted);
    set_enum(object, "replaced", svn::status_kind::replaced);
    set_enum(object, "modified", svn::status_kind::modified);
    set_enum(object, "conflicted", svn::status_kind::conflicted);
    set_enum(object, "ignored", svn::status_kind::ignored);
    set_enum(object, "obstructed", svn::status_kind::obstructed);
    set_enum(object, "external", svn::status_kind::external);
    set_enum(object, "incomplete", svn::status_kind::incomplete);

    exports["StatusKind"].set(object, no::property_attribute::read_only);
}
} // namespace status_kind
} // namespace no
