#ifndef CIRCUITELEMENT_H
#define CIRCUITELEMENT_H

#include <string>

//! element type
enum CircuitElementType {INST, WIRE, PORT};


/*!
 * Base class for wire, intance, and port types.
 * Contains basics information for name and type
*/
class CircuitElement {
  public:
    CircuitElement(std::string name_) : name(name_), visited(false) {}
    std::string get_name() const
    {
        return name;
    }
    virtual CircuitElementType get_type() const = 0;
    void set_visited(bool val)
    {
        visited = val;
    }
    bool is_visited() const
    {
        return visited;
    }
        
  private:
    std::string name;
    bool visited;
};

#endif
