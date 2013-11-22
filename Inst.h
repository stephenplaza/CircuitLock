#ifndef INST_H
#define INST_H

#include "TechLibrary.h"
#include "CircuitElement.h"
#include <vector>

class Port;

class Inst : public CircuitElement {
  public:
    Inst(std::string name_, bool is_port_ = false, bool is_latch_ = false) : 
        CircuitElement(name_), is_port(false), is_latch(false), visited(false) {}

    CircuitElementType get_type() const
    {
        return INST;
    }

    void add_input(Port* in_port)
    {
        inputs.push_back(in_port);
    }
    void add_output(Port* in_port)
    {
        outputs.push_back(in_port);
    }

    void evaluate(int num_simulations);
    void evaluate_core(int num_simulations);

    void add_lib_cell(lib_cell* lib_cell_)
    {
        lib_cells.push_back(lib_cell_);
    } 

    bool get_is_port() const
    {
        return is_port;
    }
    bool get_is_latch() const
    {
        return is_port;
    }

    unsigned int num_outputs() const
    {
        return outputs.size();
    }

    unsigned int num_inputs() const
    {
        return inputs.size();
    }   

    Port * get_output(unsigned int id)
    {
        return outputs[id];
    }
    Port * get_input(unsigned int id)
    {
        return inputs[id];
    }

    typedef std::vector<Port*>::iterator output_iterator;
    typedef std::vector<Port*>::iterator input_iterator;

    output_iterator output_begin()
    {
       return output_iterator(outputs.begin()); 
    }
    output_iterator output_end()
    {
       return output_iterator(outputs.end()); 
    }

    input_iterator input_begin()
    {
       return input_iterator(inputs.begin()); 
    }
    input_iterator input_end()
    {
       return input_iterator(inputs.end()); 
    }

    lib_cell* get_libcell(unsigned int id)
    {
        return lib_cells[id];
    }

    bool get_visited() const
    {
        return visited;
    }

    void set_visited(bool val)
    {
        visited = val;
    }

  private:
    std::vector<Port*> inputs;
    std::vector<Port*> outputs;

    //! vector of single output library function (corresponds to outputs)
    std::vector<lib_cell*> lib_cells;
   
    bool is_port;
    bool is_latch;
    
    //! general flag for algorithms that iterate through instances
    bool visited;
    
    int level; // Level of the instance
    
};

#endif
