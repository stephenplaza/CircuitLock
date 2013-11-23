#include "Wire.h"
#include "Port.h"
#include "Inst.h"

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
 
// will add wire's outputs to this
void Wire::reassign_outputs(Wire* wire)
{
    for (int i = 0; i < wire->outputs.size(); ++i) {
        add_output_port(wire->outputs[0]);
    }
    wire->outputs.clear();
}

bool Wire::is_output()
{
    for (int i = 0; i < outputs.size(); ++i) {
        if (outputs[i]->get_inst()->get_is_port() ||
            outputs[i]->get_inst()->get_is_latch()) {
            return true;
        }
    }
    return false;
}
