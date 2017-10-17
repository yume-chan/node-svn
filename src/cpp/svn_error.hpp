#pragma once

#include <string>

#include <apr_errno.h>

namespace svn {
class svn_error : public std::runtime_error {
  public:
    explicit svn_error(apr_status_t code,
                       const char*  what,
                       svn_error*   child = nullptr,
                       std::string  file  = std::string(),
                       int          line  = -1);

    ~svn_error();

    apr_status_t code;
    svn_error*   child;
    std::string  file;
    long         line;
};
} // namespace svn
