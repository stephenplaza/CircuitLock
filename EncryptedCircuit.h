#ifndef ENCRYPTEDCIRCUIT_H
#define ENCRYPTEDCIRCUIT_H

#include "Circuit.h"

typedef std::string Key;

/*!
 * Encryption algorithms will not obfucate primary (or latch) input/output wires.
*/
class EncryptedCircuit : public Circuit {
  public:
    EncryptedCircuit(std::string filename, TechLibrary* library_) :
        Circuit(filename, library_) {}

    /*!
     * Adds XOR locking gates randomly to the circuit.  There will
     * be an error if one attempts to add too many XORs.
    */
    void add_random_xors(int num_xors);
   
    /*!
     * Adds MUXes between two signals to preserve testing.
    */
    void add_test_mux(int num_muxes, bool get_cands);
 
    /*!
     * Prints the key wire and its unlocking value.
    */
    void print_keys();
  
    /*!
     * Set the key to a value.
    */ 
    void toggle_key(unsigned int key_id);

    /*!
     * Randomly set the value of all of the keys.
    */
    void randomly_set_keys();

    /*!
     * Correctly set keys.
    */
    void correctly_set_keys();

    void print_testability_prob(std::tr1::unordered_set<Inst*>& stuck0,
        std::tr1::unordered_set<Inst*>& stuck1);

    int get_num_keys() const
    {
        return key_wires.size();
    }
    
    bool get_key_value(unsigned int id);
    bool get_current_key_value(unsigned int id);

    void levelize();
 
    std::vector<Inst*> get_new_gates()
    {
        return new_gates;
    }

  private:
    Inst* create_cover(Inst* inst_correct, Inst* inst_cover, CoverType cover);

    void insert_mux(Inst* inst, Inst* cover, std::string key_name, int value);

    void insert_xor(Inst* inst, std::string name, int value);

    CoverType find_cover(Wire* wire1, Wire* wire2, Circuit& validation_circuit);
  
    //! these are new inputs to the circuit (not in base class PI list)
    std::vector<Wire*> key_wires;

    //! the unlocking key
    std::tr1::unordered_map<Key, int> key_values;

    std::vector<Inst*> new_gates;
};

#endif
