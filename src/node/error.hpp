#include <stdexcept>

namespace no {
struct type_error : public std::runtime_error {
  public:
    type_error(const char* what) noexcept
        : runtime_error(what) {}
};
} // namespace no
