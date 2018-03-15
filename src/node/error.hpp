#pragma once

#include <stdexcept>

namespace no {
class type_error : public std::runtime_error {
  public:
    type_error(const char* what) noexcept
        : runtime_error(what) {}
};
} // namespace no
