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
    for (unsigned int i = 0; i < wire->outputs.size(); ++i) {
        add_output_port(wire->outputs[0]);
    }
    wire->outputs.clear();
}

int Wire::sig_diffs(Wire& wire1)
{
    assert(signatures.size() == wire1.signatures.size());
    int diffs = 0;
    for (int i = 0; i < int(signatures.size()); ++i) {
        unsigned long long mask = 1;
        for (int j = 0; j < int(SIGSTEP); ++j) {
            unsigned long long val = signatures[i] & (mask << j);
            unsigned long long val1 = wire1.signatures[i] & (mask << j);
            if (val != val1) {
                ++diffs;
            }
        }
    }
    return diffs;
} 

bool Wire::sig_equiv(Wire& wire1)
{
    assert(signatures.size() == wire1.signatures.size());

    for (int i = 0; i < int(signatures.size()); ++i) {
        if (signatures[i] != wire1.signatures[i]) {
            return false;
        }
    }
    return true;
} 

void Wire::randomize()
{
    int num_sims = sizeof(unsigned long long) * 8;
    sig_temp = 0;

    for (int i = 0; i < num_sims; ++i) {
       sig_temp |= ((unsigned long long)((rand()%2)) << i);
    }
}

bool Wire::is_output()
{
    for (unsigned int i = 0; i < outputs.size(); ++i) {
        if (outputs[i]->get_inst()->get_is_port() ||
            outputs[i]->get_inst()->get_is_latch()) {
            return true;
        }
    }
    return false;
}
