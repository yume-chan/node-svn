#pragma once

#include <stdexcept>
#include <string>

namespace svn {
class svn_error : public std::runtime_error {
  public:
    explicit svn_error(int32_t     code,
                       const char* what,
                       svn_error*  child = nullptr,
                       std::string file  = std::string(),
                       int32_t     line  = -1) noexcept;
    svn_error(const svn_error& other) noexcept;
    svn_error(svn_error&& other) noexcept;

    ~svn_error();

    int32_t     code;
    svn_error*  child;
    std::string file;
    int32_t     line;
};
} // namespace svn
