#ifndef CRACKKEY_H
#define CRACKKEY_H

#include <vector>

class Circuit;
class EncryptedCircuit;

class CrackKey {
  public:
    CrackKey(Circuit* unlocked_circuit_, EncryptedCircuit* locked_circuit_) :
        unlocked_circuit(unlocked_circuit_), locked_circuit(locked_circuit_),
        num_iterations(0), num_input_patterns(0), num_restarts(0) {}
    
    /*!
     * Runs algorithms to crack key.  A key is considered to be 'correct' if
     * it admits all test vectors and a number of random vectors given by
     * num_vec_verify.
    */
    bool generate_key(std::vector<bool>& key_values,
        int rand_sim = 0, bool use_test = true, int rand_seed = 0);

    void print_info();

    void simulate_circuits(bool use_test, bool use_rand);

  private:
    Circuit* unlocked_circuit;
    EncryptedCircuit* locked_circuit;

    int num_iterations;
    int num_input_patterns;
    int num_restarts; 
};


#endif
