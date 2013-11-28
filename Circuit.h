#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>
#include <iosfwd>
#include <fstream>
#include "Wire.h"

class CircuitElement;
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

    /*!
     * Basic simulation of primary inputs.
    */
    void simulate(int num_sims = SIGSTEP);

    /*!
     * Checks for equivalence of signatures between circuits.
    */
    virtual bool circuit_sig_equiv(Circuit& ckt1);

    void clear_signatures();
    void commit_signatures();

    /*!
     * Takes a series of test vectors (1st line is a list of PI
     * names and the remaining lines are 0's and 1's with no spaces)
    */
    void load_test_vectors(std::string testfile);
    void print_info();
   
    bool observable_signal(Inst* inst);

    /*!
     * Levelization requires that the input_wires vector
     * be initialized.
    */
    virtual void levelize();

    void load_test_vectors(const char* testfile);

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

    int sim_patterns;

    //! blif file stream 
    std::fstream ifile;
 

    std::vector<Wire*> constants_list;
    std::tr1::unordered_set<std::string> one_list;
    std::tr1::unordered_set<std::string> zero_list;

    //! includes latch outputs as primary inputs
    std::vector<Wire*> input_wires;

    //! includes latch inputs as primary outputs
    std::vector<Wire*> output_wires;

    std::vector<std::vector<unsigned long long> > input_vecs;

};





#endif
