#include "svn_error.hpp"

namespace svn {
svn_error::svn_error(apr_status_t code,
                     const char*  what,
                     svn_error*   child,
                     std::string  file,
                     int          line)
    : std::runtime_error(what)
    , code(code)
    , child(child)
    , file(file)
    , line(line) {
}

svn_error::~svn_error() {
    delete child;
}
} // namespace svn
