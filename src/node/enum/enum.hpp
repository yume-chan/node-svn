#pragma once

#include <cpp/types.hpp>

#define SET_ENUM(target, prefix, name)                                                                       \
    {                                                                                                        \
        auto key   = no::NewName(isolate, #name);                                                            \
        auto value = static_cast<int32_t>(prefix::name);                                                     \
        target->DefineOwnProperty(context,                                                                   \
                                  key,                                                                       \
                                  no::New(isolate, value),                                                   \
                                  no::PropertyAttribute::ReadOnlyDontDelete);                                \
        target->DefineOwnProperty(context,                                                                   \
                                  no::New(isolate, std::to_string(value), v8::NewStringType::kInternalized), \
                                  key,                                                                       \
                                  no::PropertyAttribute::ReadOnlyDontDelete);                                \
    }

#define SET_READONLY(object, name, value)                   \
    (object)->DefineOwnProperty(context,                    \
                                no::NewName(isolate, name), \
                                value,                      \
                                no::PropertyAttribute::ReadOnlyDontDelete)

#define SET_NODE_KIND(name) SET_ENUM(object, svn::node_kind, name)
