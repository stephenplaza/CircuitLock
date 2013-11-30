#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>

/*!
 * Used for general exception handling.
*/
struct Error{
    Error(const std::string in_msg="") : msg(in_msg) {}
    const std::string msg;
};

class ScopeTime {
  public:
    ScopeTime(bool debug_ = true) : debug(debug_)
    {
        initial_time = clock();
    }
    ~ScopeTime()
    {
        if (debug) {
            std::cout << "Time Elapsed: " << getElapsed() << " seconds" << std::endl;
        }
    }
    double getElapsed()
    {
        return (clock() - initial_time) / double(CLOCKS_PER_SEC);
    }
  private:
    clock_t initial_time;
    bool debug;
};



#endif
