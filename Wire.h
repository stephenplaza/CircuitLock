#ifndef WIRE_H
#define WIRE_H

#include "CircuitElement.h"
#include <vector>

#define SIGSTEP (sizeof(unsigned long long)*8)

class Port;

class Wire : public CircuitElement {
  public:
    Wire(std::string name_) : CircuitElement(name_), driver(0), sig_temp(0) {}

    CircuitElementType get_type() const
    {
        return WIRE;
    }

    Port* get_driver()
    {
        return driver;
    }
    void set_driver(Port* in_port);
    
    void add_output_port(Port* port);
 
    bool is_output();

    unsigned int num_outputs() const
    {
        return outputs.size();
    }

    /*!
     * Reassign the output to the given wire to
     * the current object instance.
    */
    void reassign_outputs(Wire* wire);

    typedef std::vector<Port*>::iterator output_iterator;

    output_iterator output_begin()
    {
       return output_iterator(outputs.begin()); 
    }
    output_iterator output_end()
    {
       return output_iterator(outputs.end()); 
    }
    
    unsigned long long get_sig_temp() const
    {
        return sig_temp;
    }

    void set_sig_temp(unsigned long long sig_temp_)
    {
        sig_temp = sig_temp_;
    }
    
    int num_sig_spots()
    {
        return signatures.size();
    }

    void commit_signature()
    {
        signatures.push_back(sig_temp);
    }

    void clear_signature()
    {
        signatures.clear();
    }

    int sig_diffs(Wire& wire1);
    bool sig_equiv(Wire& wire1); 
    
    void randomize();

    unsigned long long get_signature(int i)
    {
        return signatures[i];
    }
    
    std::vector<unsigned long long> get_signature()
    {
        return signatures;
    }

  private:
    std::vector<Port*> outputs;
    Port* driver;

    //! contains value from simulation, not yet saved to signatures
    unsigned long long sig_temp;
    
    std::vector<unsigned long long> signatures;

};

#endif
