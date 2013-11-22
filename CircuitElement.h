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
    CircuitElement(std::string name_) : name(name_) {}
    std::string get_name() const
    {
        return name;
    }
    virtual CircuitElementType get_type() const = 0;

  private:
    std::string name;
};

#endif
