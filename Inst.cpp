#include "Inst.h"
#include "Port.h"
#include "Wire.h"

using std::vector; using std::string;

//cannot handle more than 64 simulations
void Inst::evaluate(int num_simulations)
{
    Port* port2;
    Port* port3;
    Wire* wire2;
    Wire* wire3;
    int minterm;
    lib_cell* lcell;

    int size = inputs.size();
    assert(num_simulations <= 64);

    // For more than 8 inputs, do not use table to save memory
    if (size > 5) {
        //make sure to clear sig_temp
        evaluate_core(num_simulations);
        return;
    }

    //solve internally 

    unsigned long type;

    unsigned long long sim1;
    unsigned long long sim2;
    unsigned long long sim3;
    //cout << get_name() << endl;

    if(inputs.size() > 1) {
        if(inputs[0]->get_wire()) {
            sim1 = inputs[0]->get_wire()->get_sig_temp();
        }
        else {
            sim1 = (unsigned long long) 0;
        }

        if(inputs[1]->get_wire()) {
            sim2 = inputs[1]->get_wire()->get_sig_temp();
        } else {
            sim2 = (unsigned long long) 0;
        }

        if(inputs.size() > 2) { 
            if(inputs[2]->get_wire()) {
                sim3 = inputs[2]->get_wire()->get_sig_temp();
            } else {
                sim3 = (unsigned long long) 0;
            }
        }
    }  

    bool do_exhaustive;    
    int i = 0;
    for (vector<Port*>::iterator pport= outputs.begin();
            pport != outputs.end(); pport++) {
        do_exhaustive = false;
        port2= *pport;
        wire2= port2->get_wire();

        if(!wire2) continue;
        wire2->set_sig_temp(0);

        type = lib_cells[i]->get_table_value();
        if(inputs.size() == 2) {
            switch (type)
            {
                case 6: // XOR
                    {
                        wire2->set_sig_temp(sim1 ^ sim2);
                        break;
                    }
                case 9: // XNOR
                    {
                        wire2->set_sig_temp(sim1 ^ (~sim2));
                        break;
                    }
                case 7: // NAND 
                    {
                        wire2->set_sig_temp(~(sim1 & sim2));
                        break;
                    }
                case 13: // NANDL 
                    {
                        wire2->set_sig_temp(~((~sim1) & sim2));
                        break;
                    }
                case 11: // NANDR 
                    {
                        wire2->set_sig_temp(~(sim1 & (~sim2)));
                        break;
                    }
                case 1: // NOR
                    {
                        wire2->set_sig_temp(~(sim1 | sim2));
                        break;
                    }
                case 4: // NORL
                    {
                        wire2->set_sig_temp(sim1 & (~sim2));
                        break;
                    }
                case 2: // NORR
                    {
                        wire2->set_sig_temp((~sim1) & sim2);
                        break;
                    }
                case 8: // AND
                    {
                        wire2->set_sig_temp(sim1 & sim2);
                        break;
                    }
                case 14: // OR
                    {
                        wire2->set_sig_temp(sim1 | sim2);
                        break;
                    }
                default:
                    {
                        do_exhaustive = true;
                    }
            }
        }
        else if(inputs.size() == 3) {
            switch(type) {
                case 216: // mux
                    {
                        wire2->set_sig_temp(((~sim3) & sim1) | (sim3 & sim2));
                        break;
                    }
                default:
                    {
                        do_exhaustive = true;
                    }
            }
        } else {
            do_exhaustive = true;
        }

        if(do_exhaustive) {
            //num_inefficient_simulations++;
            for(int k = 0; k < num_simulations; k++) {
                // For <=8 inputs, use truth table
                minterm= 0;
                for (vector<Port*>::iterator pport2= inputs.begin();
                        pport2 != inputs.end(); pport2++) {
                    minterm= minterm << 1;
                    port3= *pport2;
                    wire3= port3->get_wire();
                    // if not connected, it is redundant. Always assume it is 0
                    if (wire3 && (wire3->get_sig_temp() & (((unsigned long long) 1) << k)) != 0)
                        minterm|= 1;
                }
                lcell= lib_cells[i];
                wire2->set_sig_temp(wire2->get_sig_temp() | 
                        (((unsigned long long) (lcell->get_minterm(minterm) ? 1 : 0)) << k));
            }
        }

        if(num_simulations < 64) {
            wire2->set_sig_temp(wire2->get_sig_temp() & 
                    (((unsigned long long)1 << (num_simulations)) - ((unsigned long long) 1)));
        }

        ++i; 
    }
}

// Evaluate input wires and update output wire
void Inst::evaluate_core(int num_simulations)
{
    string inputstr;
    vector<Port*>::iterator pport;
    Port* port2;
    Wire* wire2;
    int i;

    for(int j = 0; j < num_simulations; j++) {
        inputstr = "";
        for (pport= inputs.begin(); pport != inputs.end(); pport++)
        {
            port2= *pport;
            wire2= port2->get_wire();
            if ((wire2->get_sig_temp() & (((unsigned long long) 1) << j)) == 0)
                inputstr= inputstr + '0';
            else if ((wire2->get_sig_temp() & (((unsigned long long) 1 << j))) != 0)
                inputstr= inputstr + '1';
        }

        for (i = 0, pport= outputs.begin(); pport != outputs.end(); pport++, i++) {
            port2= *pport;
            wire2= port2->get_wire();
            if(!wire2) continue;

            if(j == 0) {
                wire2->set_sig_temp(0);
            }

            wire2->set_sig_temp(wire2->get_sig_temp() | 
                    ((unsigned long long) lib_cells[i]->simulate(inputstr)) << j);
        }
    }


    if(num_simulations < 64) {
        for (pport= outputs.begin(); pport != outputs.end(); pport++) {
            port2= *pport;
            wire2= port2->get_wire();
            if(!wire2) continue;
            wire2->set_sig_temp(wire2->get_sig_temp() & 
                    (((unsigned long long)1 << (num_simulations)) - 1));
        }
    } 
}


