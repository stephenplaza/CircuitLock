#ifndef UTILS_H
#define UTILS_H

#include <string>

/*!
 * Used for general exception handling.
*/
struct Error{
    Error(const std::string in_msg="") : msg(in_msg) {}
    const std::string msg;
};

#endif
