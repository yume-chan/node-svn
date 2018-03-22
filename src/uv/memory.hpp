#pragma once

#include <uv.h>

#include "error.hpp"

namespace uv {
static int64_t resident_set_memory() {
    size_t value;
    check_result(uv_resident_set_memory(&value));
    return value;
}
} // namespace uv
