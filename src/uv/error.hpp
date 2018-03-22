#pragma once

#include <stdexcept>

#include <uv.h>

static void check_result(int error) {
    if (error != 0) {
        throw std::runtime_error(uv_strerror(error));
    }
}
