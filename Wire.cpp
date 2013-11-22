#include "Wire.h"
#include "Port.h"

// add port. isw=0: input, 1: output
void Wire::add_output_port(Port* port)
{
    outputs.push_back(port);
    port->set_wire(this);
}

void Wire::set_driver(Port* in_port)
{
    driver = in_port;
    in_port->set_wire(this);
}
