#pragma once

#include <string>
#include <cstring>

#include <node/v8.hpp>
#include <node/error.hpp>

static std::string convert_string(const v8::Local<v8::Value>& value) {
    if (!value->IsString())
        throw no::type_error("cannot convert argument to string");

    v8::String::Utf8Value utf8(value);
    auto                  length = static_cast<size_t>(utf8.length());

    if (std::strlen(*utf8) != length)
        throw no::type_error("cannot convert argument with null characters to string");

    return std::string(*utf8, length);
}
