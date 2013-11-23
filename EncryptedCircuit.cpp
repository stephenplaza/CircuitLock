#include "EncryptedCircuit.h"
#include "utils.h"
#include "Wire.h"
#include "Inst.h"
#include "Port.h"

#include <cmath>
#include <iostream>
#include <sstream>


using std::cout; using std::endl;
using std::vector; using std::tr1::unordered_map;
using std::stringstream; using std::string;

const char * XOR_BLIF = "10 1\n01 1\n";
const char * XNOR_BLIF = "11 1\n00 1\n";

void EncryptedCircuit::print_keys()
{
    for (unordered_map<Key, int>::iterator iter = key_values.begin();
            iter != key_values.end(); ++iter) {
        cout << iter->first << "(" << iter->second << ")" << " ";
    }
    cout << endl;
}

void EncryptedCircuit::set_key(Key key, int value)
{
    if (sym_table.find(key) == sym_table.end()) {
        throw Error("Requested key does not exist");
    }
    
    Wire* wire = (Wire*)(sym_table[key]);
    if (value > 0) {
        wire->set_sig_temp(~((unsigned long long)(0)));
    } else {
        wire->set_sig_temp(((unsigned long long)(0)));
    }
}

void EncryptedCircuit::randomly_set_keys()
{
    for (int i = 0; i < key_wires.size(); ++i) {
        if (rand()%2) {
            key_wires[i]->set_sig_temp(~((unsigned long long)(0)));
        } else {
            key_wires[i]->set_sig_temp(((unsigned long long)(0)));
        }
    }
}

void EncryptedCircuit::add_random_xors(int num_xors)
{
    int num_ops = linsts.size();
    if (num_ops < num_xors) {
        throw Error("Request for more keys than gates");
    }

    vector<Inst*> chosen_insts;
    while (num_xors > 0) {
        Inst* inst = linsts[(rand() % (linsts.size()))];
        if (inst->is_visited()) {
            continue;
        }
        Wire* owire = inst->get_output(0)->get_wire();
        if (owire->is_output()) {
            --num_ops;
            if (num_ops < num_xors) {
                throw Error("Request for more keys than gates");
            }
            continue;
        } 

        // ?! TBD: verify that an XOR impacts the output
        chosen_insts.push_back(inst);
        inst->set_visited(true);
        --num_xors;
    }

    // march through list and randomly choose value, insert logic
    for (int i = 0; i < chosen_insts.size(); ++i) {
        stringstream keyss;
        keyss << "key-" << i;
        string key = keyss.str();
        int value = rand() % 2;
        key_values[key] = value;
   
        // value = 0 add an XOR; value = 1 add an XNOR
        // adds wire to key_wires 
        insert_xor(chosen_insts[i], key, value);
        
        // clear flags
        chosen_insts[i]->set_visited(false);
    }

    levelize();
}

void EncryptedCircuit::levelize()
{
    // key wires are not added to the circuit input list
    // which is used for levelization
    for (int i = 0; i < key_wires.size(); ++i) {
        key_wires[i]->set_visited(true);
    } 
    Circuit::levelize();
    for (int i = 0; i < key_wires.size(); ++i) {
        key_wires[i]->set_visited(false);
    } 
}

void EncryptedCircuit::insert_xor(Inst* inst, string key_name, int value)
{
    // create XOR/XNOR instance
    string key_out = key_name + "-xor";
    string inst_name = key_out+"name";
    Inst* new_inst = new Inst(inst_name); 
    lib_insts.push_back(new_inst);
    ++num_insts;
    ++num_gates;
    sym_table[inst_name] = new_inst;
    lib_cell* cell;
    if (value) {
        cell = library->create_libcell(XNOR_BLIF, 2);
    } else {
        cell = library->create_libcell(XOR_BLIF, 2);
    }
    new_inst->add_lib_cell(cell);

    // create output port and assign wire
    Port *oport = new Port(key_out);
    ++num_ports;
    oport->set_inst(new_inst);
    new_inst->add_output(oport);
    Wire* owire = new Wire(key_out);
    ++num_wires;
    sym_table[key_out] = owire;
    owire->set_driver(oport);

    // grab inst output wire
    if (inst->num_outputs() != 1) {
        throw Error("Number of outputs should be one");
    }    
    Wire* reconnect_wire = inst->get_output(0)->get_wire();

    // create XOR key input and wire
    Port *port1 = new Port(key_name);
    ++num_ports;
    new_inst->add_input(port1); 
    port1->set_inst(new_inst);
    Wire* wire1 = new Wire(key_name);
    // make sure key wire is marked as visited; should not be visited
    ++num_wires;
    sym_table[key_name] = wire1;
    wire1->add_output_port(port1);
    key_wires.push_back(wire1);  

    // create XOR second input
    string xor_byname = key_name + "-by";
    Port *port2 = new Port(xor_byname);
    ++num_ports;
    new_inst->add_input(port2);
    port2->set_inst(new_inst);

    // move reconnect wire output to owire
    owire->reassign_outputs(reconnect_wire); 

    // add original wire output to XOR input 2
    reconnect_wire->add_output_port(port2); 

    // create key input
    string inst_inname = key_name + "_input";
    Inst* inst_in = new Inst(inst_inname, true);
    ++num_insts;
    sym_table[inst_inname] = inst_in;
    oport = new Port(key_name);
    ++num_ports;
    inst_in->add_output(oport);
    oport->set_inst(inst);
    wire1->set_driver(oport);
}


