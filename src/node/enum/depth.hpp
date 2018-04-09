#pragma once

#include "enum.hpp"

namespace no {
namespace depth {
void initialize(no::object& exports) {
    no::object object(exports.isolate());
    set_enum(object, "unknown", svn::depth::unknown);
    set_enum(object, "empty", svn::depth::empty);
    set_enum(object, "files", svn::depth::files);
    set_enum(object, "immediates", svn::depth::immediates);
    set_enum(object, "infinity", svn::depth::infinity);

    exports["Depth"].set(object, no::property_attribute::read_only);
}
} // namespace depth
} // namespace no
