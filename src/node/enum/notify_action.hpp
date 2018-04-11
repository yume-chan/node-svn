#pragma once

#include "enum.hpp"

namespace no {
namespace notify_action {
void initialize(no::object& exports) {
    no::object object(exports.isolate());
    set_enum(object, "delete", svn::notify_action::update_delete);
    set_enum(object, "add", svn::notify_action::update_add);
    set_enum(object, "update", svn::notify_action::update_update);
    set_enum(object, "completed", svn::notify_action::update_completed);
    set_enum(object, "external", svn::notify_action::update_external);
    set_enum(object, "replace", svn::notify_action::update_replace);
    set_enum(object, "started", svn::notify_action::update_started);
    set_enum(object, "skip_obstruction", svn::notify_action::update_skip_obstruction);
    set_enum(object, "skip_working_only", svn::notify_action::update_skip_working_only);
    set_enum(object, "skip_access_denied", svn::notify_action::update_skip_access_denied);
    set_enum(object, "external_removed", svn::notify_action::update_external_removed);
    set_enum(object, "shadowed_add", svn::notify_action::update_shadowed_add);
    set_enum(object, "shadowed_update", svn::notify_action::update_shadowed_update);
    set_enum(object, "shadowed_delete", svn::notify_action::update_shadowed_delete);

    exports["UpdateNotifyAction"].set(object, no::property_attribute::read_only);
}
} // namespace notify_action
} // namespace no
