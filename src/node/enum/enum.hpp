#pragma once

#include <cpp/types.hpp>

#include <objects/object.hpp>

template <size_t N, class T>
void set_enum(no::object& object, const char (&name)[N], T value) {
    auto number = static_cast<int32_t>(value);
    object[name].set(number, no::property_attribute::read_only | no::property_attribute::dont_delete);
    object[std::to_string(number)].set(name, no::property_attribute::read_only | no::property_attribute::dont_delete);
}
