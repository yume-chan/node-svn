#pragma once

#include <node/v8.hpp>

namespace no {
namespace revision_kind {
void initialize(no::object& exports) {
    no::object object(exports.isolate());
    set_enum(object, "unspecified", svn::revision_kind::unspecified);
    set_enum(object, "number", svn::revision_kind::number);
    set_enum(object, "date", svn::revision_kind::date);
    set_enum(object, "committed", svn::revision_kind::committed);
    set_enum(object, "previous", svn::revision_kind::previous);
    set_enum(object, "base", svn::revision_kind::base);
    set_enum(object, "working", svn::revision_kind::working);
    set_enum(object, "head", svn::revision_kind::head);

    exports["RevisionKind"].set(object, no::property_attribute::read_only);
}
} // namespace revision_kind
} // namespace no
