#pragma once

#include "enum.hpp"

namespace no {
static void is_commit_finalize_notify(const v8::FunctionCallbackInfo<v8::Value>& args) {
    no::object notify(args[0].As<v8::Object>());

    auto action = notify["action"].as<v8::Number>()->Int32Value();
    args.GetReturnValue().Set(no::data(args.GetIsolate(), action == static_cast<int32_t>(svn::notify_action::commit_finalizing)));
}

namespace notify_action {
void initialize(no::object& exports) {
    no::object update(exports.isolate());

    set_enum(update, "delete", svn::notify_action::update_delete);
    set_enum(update, "add", svn::notify_action::update_add);
    set_enum(update, "update", svn::notify_action::update_update);
    set_enum(update, "completed", svn::notify_action::update_completed);
    set_enum(update, "external", svn::notify_action::update_external);
    set_enum(update, "replace", svn::notify_action::update_replace);
    set_enum(update, "started", svn::notify_action::update_started);
    set_enum(update, "skip_obstruction", svn::notify_action::update_skip_obstruction);
    set_enum(update, "skip_working_only", svn::notify_action::update_skip_working_only);
    set_enum(update, "skip_access_denied", svn::notify_action::update_skip_access_denied);
    set_enum(update, "external_removed", svn::notify_action::update_external_removed);
    set_enum(update, "shadowed_add", svn::notify_action::update_shadowed_add);
    set_enum(update, "shadowed_update", svn::notify_action::update_shadowed_update);
    set_enum(update, "shadowed_delete", svn::notify_action::update_shadowed_delete);

    exports["UpdateNotifyAction"].set(update, no::property_attribute::read_only);

    no::object commit(exports.isolate());

    set_enum(commit, "modified", svn::notify_action::commit_modified);
    set_enum(commit, "added", svn::notify_action::commit_added);
    set_enum(commit, "deleted", svn::notify_action::commit_deleted);
    set_enum(commit, "replaced", svn::notify_action::commit_replaced);
    set_enum(commit, "postfix_txdelta", svn::notify_action::commit_postfix_txdelta);
    set_enum(commit, "finalizing", svn::notify_action::commit_finalizing);

    exports["CommitNotifyAction"].set(commit, no::property_attribute::read_only);

    exports["is_commit_finalize_notify"] = no::data<v8::Function>(exports.context(), is_commit_finalize_notify);
}
} // namespace notify_action
} // namespace no
