#include "Circuit.h"

#include "Wire.h"
#include "Inst.h"
#include "Port.h"
#include "TechLibrary.h"

#include "utils.h"

#include <iostream>
#include <list>
#include <cmath>
#include <algorithm>
#include <cassert>

#include <fstream>
#include <string>
#include <map>

using std::cout; using std::endl;
using std::string; using std::vector;
using std::ifstream; using std::ofstream;
using std::tr1::unordered_set;
using std::map;

Circuit::Circuit(string filename, TechLibrary* library_) :
    library(library_), num_insts(0), num_wires(0), num_gates(0),
    num_ports(0), max_level(0)
{
    // will throw an Error if incorrectly formatted
    parse_blif(filename);

    levelize();
}

/*!
 * Assumes all of the wire/inst visited flags are false
*/
void Circuit::levelize()
{
    max_level= 0;
    linsts.clear();
    
    for (vector<Wire*>::iterator pwire = input_wires.begin();
            pwire != input_wires.end(); ++pwire) {
	(*pwire)->set_visited(true);
    }

    // Add all latch and inputs to linst; reset level
    for (vector<Inst*>::iterator pinst= lib_insts.begin();
            pinst != lib_insts.end(); pinst++) {
        (*pinst)->set_level(0);
	if ((*pinst)->get_is_latch() || (*pinst)->is_PI()) {
	    (*pinst)->set_visited(true);
	    linsts.push_back(*pinst);
	}
    }

    // Now check all instances, if all its input is valid, add to linsts
    unsigned int num_iterations = 0;
    while (linsts.size() < lib_insts.size())
    {
        ++num_iterations;
        // more iterations than instances == problem
        if (num_iterations > lib_insts.size()) {
            throw Error("Cannot levelize instances");
            break;
        }

        for (vector<Inst*>::iterator pinst= lib_insts.begin();
                pinst != lib_insts.end(); pinst++) {
            Inst* inst2= *pinst;
            if (inst2->is_visited()) {
                continue;
            }
            int max_level2= 0;
            Inst::input_iterator pport;
            for (pport = inst2->input_begin();
                    pport != inst2->input_end(); ++pport) {
                Wire* wire2= (*pport)->get_wire();
                if (wire2 && !(wire2->is_visited())) {
                    break;
                }
                int curr_level = wire2->get_driver()->get_inst()->get_level();
                if (max_level2 < curr_level) {
                    max_level2 = curr_level;
                }
            }

            if (pport == inst2->input_end()) {
                linsts.push_back(inst2);
                ++max_level2;
                inst2->set_level(max_level2);
                if (max_level < max_level2) {
                    max_level= max_level2;
                }
                inst2->set_visited(true);
                for (Inst::output_iterator pport= inst2->output_begin();
                        pport != inst2->output_end(); pport++) {
                    Wire* wire2= (*pport)->get_wire();
                    if (wire2) {
                        wire2->set_visited(true);
                    }
                }
            }
        }
    }

    // clear all the flags, leave the level
    for (sym_map::iterator iter = sym_table.begin(); 
            iter != sym_table.end(); ++iter) {
        (iter->second)->set_visited(false);
    }
} 

void Circuit::print_info()
{
    // number of logic and sequential instances
    cout << "Num instances: " << num_insts << endl;

    // number of wires
    cout << "Num wires: " << num_wires << endl;

    // number of combinational gates
    cout << "Num of logic gates: " << num_gates << endl;

    // number of logic levels
    cout << "Num levels: " << max_level << endl;
}


// Parse the blif file and build the circuit
void Circuit::parse_blif(string filename)
{
    string latch_str= "1 1\n";

    ifile.open(filename.c_str());
    if (!ifile) {
        throw Error("Cannot open file");    
    }

    while (true) {
        int type;
        string token;
        type= get_blif_token(token);
        if (type == 2) {
            break;
        } else if (type == 4) {
            if (token == ".inputs" || token == ".outputs") {
                int type= 0;

                if (token == ".outputs") {
                    type= 1;
                }
                while (get_blif_token(token) != 1) {
                    // create wire
                    Wire* nwire= new Wire(token);
                    ++num_wires;
                    sym_table[token]= nwire;

                    // create instance
                    string instname = token;
                    instname += type == 0 ? "_input" : "_output";
                    Inst* ninst= new Inst(instname, true);
                    ++num_insts;
                    sym_table[instname]= ninst;

                    // create port
                    Port* nport= new Port(token);
                    ++num_ports;
                    nport->set_inst(ninst);
                    nport->set_wire(nwire);
                    
                    if (type == 0) {
                        // input, connect port as output
                        ninst->add_output(nport);
                        nwire->set_driver(nport);
                        input_wires.push_back(nwire);
                    } else {
                        ninst->add_input(nport);
                        nwire->add_output_port(nport);
                        output_wires.push_back(nwire);
                    }
                }
            } else if (token == ".latch") {
                // .latch input output [<type> <control>] [<init-val>]
                // only need input and output
                string input, output;
                get_blif_token(input);
                get_blif_token(output);
                
                // Exhaust other token until end-of-line
                while (get_blif_token(token) != 1);
                
                // create instance
                string inst_name = input + "_" + output + "_latch";
                Inst* ninst= new Inst(inst_name, false, true);
                ++num_insts;
                sym_table[inst_name]= ninst;
                
                // handle input port
                Port* nport= new Port(input);
                ++num_ports;
                nport->set_inst(ninst);
                
                // create/find wire 
                Wire* nwire= find_wire_insert(input);
                nwire->add_output_port(nport);
                output_wires.push_back(nwire);
                ninst->add_input(nport);
                
                // handle output port
                nport= new Port(output);
                ++num_ports;
                nport->set_inst(ninst);
                
                // create/find wire
                nwire = find_wire_insert(output);
                if (nwire->get_driver() != 0) {
                    cout << "Warning, wire " << output
                        << " has multipler drivers."<<endl;
                }
                input_wires.push_back(nwire);
                nwire->set_driver(nport);
                ninst->add_output(nport);
                // latch always has only one input
                lib_cell* cell = library->create_libcell(latch_str, 1);
                ninst->add_lib_cell(cell);
            } else if (token == ".names") {
                // .names <in-1> <in-2> ... <in-n> <output>
                vector<string> strvec;
                int i;
                string inst_name;
                
                Inst* ninst;
                Port* nport;
                Wire* nwire;

                while (get_blif_token(token) != 1) {
                    strvec.push_back(token);
                    inst_name= inst_name + token + "_";
                }

                char chr_temp= ifile.get();
                ifile.putback(chr_temp);
                if (chr_temp == '.') {
                    cout<<"Warning, instance "<<inst_name<<" has no truth table."<<endl;
                    if(strvec.size() == 1) {
                        zero_list.insert(strvec[0]);
                        nwire = find_wire_insert(strvec[0]);
                        throw Error("Found a constant 0");
                        constants_list.push_back(nwire);
                    }
                    continue;
                }
                if(strvec.size() == 1) {
                    cout << "Warning, constant wire " << inst_name << endl;
                    one_list.insert(strvec[0]);
                    nwire= find_wire_insert(strvec[0]);
                    throw Error("Found a constant 1");
                    constants_list.push_back(nwire);
                    continue;
                }

                // set instance
                inst_name= inst_name + "name";
                ninst= new Inst(inst_name);
                lib_insts.push_back(ninst);
                ++num_insts;
                ++num_gates;
                sym_table[inst_name]= ninst;
                
                // first n-1 names are inputs
                for (i= 0; i < int(strvec.size()) - 1; i++) {
                    nport= new Port(strvec[i]);
                    ++num_ports;
                    nwire= find_wire_insert(strvec[i]);
                    nwire->add_output_port(nport);
                    ninst->add_input(nport);
                    nport->set_inst(ninst);
                }
                
                // last name is output
                int size= strvec.size();
                nport= new Port(strvec[size-1]);
                ++num_ports;
                nport->set_inst(ninst);
                nwire= find_wire_insert(strvec[size-1]);
                ninst->add_output(nport);
                if (nwire->get_driver() != 0) {
                    cout<<"Warning, wire "<<strvec[size - 1]<<" has multiple drivers."<<endl;
                }
                nwire->set_driver(nport);
                
                // next one should be truth-table
                if (get_blif_ttable(token) == 1) { 
                    cout<<"Warning, instance "<<inst_name<<" has no truth table."<<endl;
                }
                lib_cell* cell = library->create_libcell(token, 
                        ninst->num_inputs());
                ninst->add_lib_cell(cell);
            } else {
                // Not handled. Skip everything until end of line
                while (get_blif_token(token) != 1) ;
            }
        }
    }
    ifile.close();
}

// return blif token in token. return value is: 0: normal token, 1: end-of-line,
// 2: end-of-file, 4: keyword(.names, .input, .end, etc)
int Circuit::get_blif_token(string& token)
{
    int chr;
    int res;
    bool in_token= false;

    token.clear();
    // skip heading white space
    while (ifile) {
        chr= ifile.get();
        if (chr != ' ' && chr != '\t')
            break;
    }
    ifile.putback(chr);
    while (ifile) {
        chr= ifile.get();
        if (chr == '#') {
            // skip to end-of-line
            do {
                chr= ifile.get();
            } while (chr != '\n' && ifile.eof() == false);
            continue;
        }
        if (chr == '\r') {
            chr= ifile.get();
        }
        if (chr == '\n') {
            if (in_token == true) {
                ifile.putback(chr);
                return res;
            } else {
                return 1;
            }
        }
        if (chr == '\\') {
            chr= ifile.get();
            if (chr == '\r') {
                chr= ifile.get();
            }
            if (chr == '\n') {
                chr= ifile.get();
            }
            if (in_token == false) {
                // skip whitespace
                while (ifile.eof() == false) {
                    if (chr != ' ' && chr != '\t') {
                        break;
                    }
                    chr= ifile.get();
                } 
            }
        }
        if (in_token && (chr == ' ' || chr == '\t')) {
            return res;
        }
        if (!in_token) {
            if (chr == '.') {
                res= 4;
            } else {
                res= 0;
            }
            in_token= true;
        }
        token= token + (char)chr;
    }
    return 2;
}

// return truth table. If it starts with ".", return 1. Otherwise return 0.
int Circuit::get_blif_ttable(string& token)
{
    int chr;
    bool skip;

    // skip heading white space
    chr= ifile.get();
    if (chr == '.')
        return 1;
    else
        ifile.putback(chr);
    token.clear();
    // Read everything until .
    skip= false;
    while (ifile && (chr= ifile.get()) != '.')
    {
        if (chr == '#') {
            // skip # until \n
            skip= true;
        }
        if (skip == false) {
            token= token + (char)chr;
        }
        if (skip == true && chr == '\n') {
            skip= false;
        }
    }
    if (ifile) {
        ifile.putback(chr);
    }
    return 0;
}

// Find a wire from symble table. If found, return it.
// Otherwise create a wire, add to symble table, and return it.
Wire* Circuit::find_wire_insert(string& name)
{
    sym_map::iterator pmap;
    Wire* nwire;
    
    pmap= sym_table.find(name);
    if (pmap == sym_table.end()) {
        nwire= new Wire(name);
        ++num_wires;
        sym_table[name]= nwire;
    } else {
        nwire= (Wire*)(*pmap).second;
    }
    return nwire;
}

// Write blif file from netlist
void Circuit::write_blif(string filename)
{
    map<string, CircuitElement*>::iterator pmap;
    CircuitElement* element;
    Inst* inst2;
    Port* port2;
    ofstream bliffile;
    vector<Port*> ports;
    Inst::input_iterator pin_iter;
    Inst::output_iterator pout_iter;
    int i;

    bliffile.open(filename.c_str());

    // extract name of the model from the given file name
    // the filename should not have any '/' or '\' characters
    string model_name = filename;
    int last_index = model_name.find_last_of("/");
    if (last_index > 0) {
        model_name = model_name.substr(last_index+1, model_name.size());
    }
    last_index = model_name.find_last_of("\\");
    if (last_index > 0) {
        model_name = model_name.substr(last_index+1, model_name.size());
    }
    last_index = model_name.find_last_of(".");
    if (last_index > 0) {
        model_name = model_name.substr(0, last_index);
    }

    map<string, CircuitElement*> sym_table_map(sym_table.begin(), sym_table.end());

    bliffile <<".model " << model_name << endl;
    // inputs
    bliffile<<".inputs ";
    for (pmap= sym_table_map.begin(); pmap != sym_table_map.end(); pmap++) {
        element = (*pmap).second;
        if (element->get_type() == INST) {
            inst2 = (Inst*)(element);
            if (inst2->get_is_port() && (inst2->num_outputs() > 0)) {
                port2 = inst2->get_output(0);
                Wire* wire = port2->get_wire();
                if(wire) {
                    if(one_list.find(wire->get_name()) != one_list.end()) {
                        continue;
                    }
                    if(zero_list.find(wire->get_name()) != zero_list.end()) {
                        continue;
                    }
                }
                if (wire && (wire->num_outputs() > 0)) {
                    bliffile<<port2->get_name()<<" ";
                }
            }
        }
    }
    bliffile<<endl;
    bliffile<<".outputs ";
    for (pmap= sym_table_map.begin(); pmap != sym_table_map.end(); pmap++) {
        element = (*pmap).second;
        if (element->get_type() == INST) {
            inst2 = (Inst*)(element);
            if (inst2->get_is_port() && (inst2->num_inputs() > 0)) {
                port2 = inst2->get_input(0);
                Wire* wire = port2->get_wire();
                if (wire && wire->get_driver()) {
                    bliffile << port2->get_name() << " ";
                }
            }
        }
    }
    bliffile<<endl;

    // write out constants
    unordered_set<string>::iterator constants_iter;
    for(constants_iter = one_list.begin();
            constants_iter != one_list.end(); constants_iter++) {
        bliffile << ".names " << *constants_iter << endl;
        bliffile << " 1" << endl;
    }
    for(constants_iter = zero_list.begin(); 
            constants_iter != zero_list.end(); constants_iter++) {
        bliffile << ".names " << *constants_iter << endl;
    }

    // names, latch
    for (pmap= sym_table_map.begin(); pmap != sym_table_map.end(); pmap++) {
        element = (*pmap).second;
        if (element->get_type() == INST) {
            inst2= (Inst*) element;
            for (i = 0; i < int(inst2->num_outputs()); ++i) {
                if (!(inst2->get_is_port())) {
                    if (inst2->get_is_latch()) {
                        bliffile<<".latch ";
                    } else {
                        bliffile<<".names ";
                    }
                    bool need_QN= false;
                    if (inst2->get_is_latch() && (inst2->num_inputs() > 1)) {
                        // print D and Q only
                    
                        for (pin_iter = inst2->input_begin();
                                pin_iter != inst2->input_end(); ++pin_iter) {
                            port2 = *pin_iter;
                            if (port2->get_name() == "D") {
                                bliffile<<port2->get_wire()->get_name()<<" ";
                                break;
                            }
                        }
                        for (pout_iter = inst2->output_begin();
                                pout_iter != inst2->output_end(); ++pout_iter) {
                            port2= *pout_iter;
                            if (port2->get_name() == "Q") {
                                bliffile<<port2->get_wire()->get_name()<<" ";
                                break;
                            }
                        }
                        if (pout_iter == inst2->output_end()) {
                            // no Q, use inst_name+_Q
                            need_QN= true;
                            bliffile << inst2->get_name() << "_Q" << " ";
                        }
                    } else {
                        for (pin_iter = inst2->input_begin();
                                pin_iter != inst2->input_end(); ++pin_iter) {
                            port2= *pin_iter;
                            if (port2->get_wire()) {
                                bliffile << port2->get_wire()->get_name() << " ";
                            } else {
                                bliffile<<"unconnected"<<" ";
                            }
                        }
                        port2= inst2->get_output(i);
                        bliffile<<port2->get_wire()->get_name()<<" ";
                    }
                    if (inst2->get_is_latch()) {
                        bliffile<<"0"<<endl;
                        if ((inst2->num_outputs() > 1) || need_QN) {
                            Wire* wire3 = 0, *wire4 = 0;
                            // print QN = ~ Q
                            for (pout_iter = inst2->output_begin();
                                    pout_iter != inst2->output_end(); ++pout_iter) {
                                port2 = *pout_iter;
                                if (port2->get_name() == "Q") {
                                    wire3 = port2->get_wire();
                                }
                            }
                            for (pout_iter= inst2->output_begin();
                                    pout_iter != inst2->output_end(); pout_iter++) {
                                port2= *pout_iter;
                                if (port2->get_name() == "QN") {
                                    wire4 = port2->get_wire();
                                }
                            }
                            if (wire4 != NULL) {
                                bliffile<<".names ";
                                if (wire3 == NULL) {
                                    bliffile << inst2->get_name() << "_Q";
                                } else {
                                    bliffile<<wire3->get_name();
                                }
                                bliffile<<" "<<wire4->get_name()<<endl;
                                bliffile<<"0 1"<<endl;
                            }
                            // for latch, always print once only even if there
                            // are multiple outputs
                            continue;
                        }
                    } else {
                        bliffile << endl;
                        bliffile << inst2->get_libcell(i)->get_blif();
                    }
                }
            }
        }
    }
    bliffile<<".end"<<endl;
    bliffile.close();
}


// Check if port is in driver's input cone. Return true if yes.
bool Circuit::check_input_cone(Port* port2, Port* driver)
{
    vector<Inst*> stack, stack2;
    vector<Inst*>::iterator pstack;
    Inst *inst3, *inst2;
    Port *port3, *port1;
    Wire *wire2;
    bool found= false;

    if (port2->get_wire() == driver->get_wire()) {
        return false;
    }

    inst2 = driver->get_inst();
    if (!(inst2->is_visited())) {
        stack.push_back(inst2);
        stack2.push_back(inst2);
        inst2->set_visited(true);
    }

    while (!stack.empty())
    {
        inst3 = stack.back();
        stack.pop_back();
        if (inst3->get_is_latch() || inst3->get_is_port()) {
            for (Inst::output_iterator pport2= inst3->output_begin();
                    pport2 != inst3->output_end(); pport2++)
            {
                wire2 = (*pport2)->get_wire();
                if (wire2 == 0) {
                    continue;
                }
                if (wire2 == port2->get_wire()) {
                    found= true;
                    break;
                }
            }
        } else {
            // traverse all its inputs
            for (Inst::input_iterator pport2= inst3->input_begin();
                    pport2 != inst3->input_end(); pport2++) {
                port3= *pport2;
                wire2= port3->get_wire();
                if (!wire2) {
                    continue;
                }
                if (wire2 == port2->get_wire()) {
                    found= true;
                    break;
                }
                port1 = wire2->get_driver();
                inst2 = port1->get_inst();
                if (!(inst2->is_visited())) {
                    inst2->set_visited(true);
                    stack.push_back(inst2);
                    stack2.push_back(inst2);
                }
            }
        }
        if (found == true) {
            break;
        }
    }
    // clear visited flags
    for (pstack= stack2.begin(); pstack != stack2.end(); pstack++) {
        (*pstack)->set_visited(false);
    }

    return found;
}


