#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>
#include <iosfwd>
#include <fstream>

class CircuitElement;
class Wire;
class Inst;
class Port;
class TechLibrary;

class Circuit {
  public:
    Circuit(std::string filename, TechLibrary* library_);
      
    void write_blif(std::string filename);
    bool check_input_cone(Port* port2, Port* driver);
   
    typedef std::vector<Inst*>::iterator inst_iterator;
    inst_iterator inst_begin()
    {
        return inst_iterator(linsts.begin());
    } 
    inst_iterator inst_end()
    {
        return inst_iterator(linsts.end());
    } 

    void print_info();
    
    /*!
     * Levelization requires that the input_wires vector
     * be initialized.
    */
    virtual void levelize();

  protected:
    TechLibrary* library;
        
    //! maps circuit elements to unique names
    typedef std::tr1::unordered_map<std::string, CircuitElement*> sym_map;
    sym_map sym_table;    
    std::vector<Inst*> linsts;
    
    //! non input/output/latch instances
    std::vector<Inst*> lib_insts;
    
    int num_insts, num_wires, num_gates, num_ports, max_level;

  private:
    void parse_blif(std::string filename);
    int get_blif_token(std::string& token);
    int get_blif_ttable(std::string& token);
    Wire* find_wire_insert(std::string& name);
    
    //! blif file stream 
    std::fstream ifile;
 

    std::vector<Wire*> constants_list;
    std::tr1::unordered_set<std::string> one_list;
    std::tr1::unordered_set<std::string> zero_list;

    //! includes latch outputs as primary inputs
    std::vector<Wire*> input_wires;

    //! includes latch inputs as primary outputs
    std::vector<Wire*> output_wires;
};





#endif
