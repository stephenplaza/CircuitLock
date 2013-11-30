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

enum ModType { FLIP, STUCK0, STUCK1 };

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
     * Basic simulation of primary inputs using random simulation.
     * Signatures are saved but the random inputs are not.
    */
    void simulate(int num_sims = SIGSTEP);

    /*!
     * Simulate the saved random simulation vectors.
    */
    void simulate_random();
   
    /*!
     * Simulate the saved test simulation vectors.
    */
    void simulate_test();

    void set_disable_signature_clear(bool flag)
    {
        disable_signature_clear = flag;
    }

    /*!
     * Create random input vectors and store.
    */
    void create_random_inputs(int num_sims = SIGSTEP);

    /*!
     * Checks for equivalence of signatures between circuits.
    */
    virtual bool circuit_sig_equiv(Circuit* ckt1);

    void clear_signatures();
    void commit_signatures();

    int get_num_test_vectors() const
    {
        return num_test_vec;
    }

    /*!
     * Takes a series of test vectors (1st line is a list of PI
     * names and the remaining lines are 0's and 1's with no spaces)
    */
    void load_test_vectors(std::string testfile);
    void print_info();

    void output_differences(Circuit* ckt1, int& num_out_mismatch, int& num_vec_mismatch);

    bool observable_signal(Inst* inst, ModType mod = FLIP);
    void print_testability();

    /*!
     * Levelization requires that the input_wires vector
     * be initialized.
    */
    virtual void levelize();

    std::vector<std::vector<unsigned long long> > get_random_inputs()
    {
        return rand_input_vecs; 
    }
   
    void set_random_inputs(std::vector<std::vector<unsigned long long> >& input_vecs2, int rand_sim)
    {
        rand_input_vecs = input_vecs2;
        num_rand_vec = rand_sim;
    } 

    void load_test_vectors(const char* testfile);
    
    bool wires_equal(std::string w1, std::string w2, CoverType type);

  protected:
    TechLibrary* library;
        
    //! maps circuit elements to unique names
    typedef std::tr1::unordered_map<std::string, CircuitElement*> sym_map;
    sym_map sym_table;    
    std::vector<Inst*> linsts;
    
    //! non input/output/latch instances
    std::vector<Inst*> lib_insts;
    
    int num_insts, num_wires, num_gates, num_ports, max_level;
    
    std::string blif_name;

  private:
    void simulate(std::vector<std::vector<unsigned long long> >& input_vectors,
            int num_sims);
    void parse_blif(std::string filename);
    int get_blif_token(std::string& token);
    int get_blif_ttable(std::string& token);
    Wire* find_wire_insert(std::string& name);

    int sim_patterns;
    int num_test_vec;
    int num_rand_vec;

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
    std::vector<std::vector<unsigned long long> > rand_input_vecs;

    bool disable_signature_clear;
};





#endif
