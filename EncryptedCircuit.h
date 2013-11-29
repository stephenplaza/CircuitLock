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

    int get_num_keys() const
    {
        return key_wires.size();
    }
    
    bool get_key_value(unsigned int id);
    bool get_current_key_value(unsigned int id);

    void levelize();
  
  private:
    void insert_xor(Inst* inst, std::string name, int value);
  
    //! these are new inputs to the circuit (not in base class PI list)
    std::vector<Wire*> key_wires;

    //! the unlocking key
    std::tr1::unordered_map<Key, int> key_values;

};

#endif
