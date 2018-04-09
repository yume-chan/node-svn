#pragma once

#include "enum.hpp"

namespace no {
namespace node_kind {
void initialize(no::object& exports) {
    no::object object(exports.isolate());
    set_enum(object, "none", svn::node_kind::none);
    set_enum(object, "file", svn::node_kind::file);
    set_enum(object, "dir", svn::node_kind::dir);
    set_enum(object, "unknown", svn::node_kind::unknown);

    exports["NodeKind"].set(object, no::property_attribute::read_only);
}
} // namespace node_kind
} // namespace no
