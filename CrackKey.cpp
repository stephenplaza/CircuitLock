#include "CrackKey.h"
#include "EncryptedCircuit.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

using std::vector;
using std::cout; using std::endl;

const int VERIFY_LIMIT = 10000;    
const int SEARCH_LIMIT = 1000000000;

// ?? allow one to specify simulation step
bool CrackKey::generate_key(vector<bool>& key_values, int rand_sim,
        bool use_test, int rand_seed)
{
    srand(rand_seed);
    bool use_rand = false;
    if (rand_sim > 0) {
        use_rand = true;
    }

    locked_circuit->randomly_set_keys();

    int num_keys = locked_circuit->get_num_keys();
    vector<bool> examined(num_keys, false);

    int num_tests = 0;

    if (use_test) {
        num_tests = locked_circuit->get_num_test_vectors(); 
    }
    
    // simulate test vectors if available
    if (use_rand) {
        locked_circuit->create_random_inputs(rand_sim);
        vector<vector<unsigned long long> > rand_vecs = locked_circuit->get_random_inputs();
        unlocked_circuit->set_random_inputs(rand_vecs, rand_sim);
        num_tests += rand_sim;    
    }
   
    // to account for the initial simulation 
    num_input_patterns = num_tests;
    
    bool found = false;
    num_iterations = 0;
    int num_examined = 0;
    int last_bad = -1;
    int num_bads = 0;

    while (num_input_patterns < SEARCH_LIMIT) {

        // in theory, outputs could be saved from previous
        // configuration (this doesn't count against num_input_patterns)
        simulate_circuits(use_test, use_rand);
        int num_bad_outputs1, num_bad_vectors1;

        locked_circuit->output_differences(unlocked_circuit, num_bad_outputs1, num_bad_vectors1);

        if (num_examined == num_keys) {
            int matches = 0;
            for (int i = 0; i < num_keys; ++i) {
                if (locked_circuit->get_key_value(i) == locked_circuit->get_current_key_value(i)) {
                    ++matches;
                }
            }
            cout << "Matches: " << matches << " remaining errors: " << 
                num_bad_outputs1 << ", " << num_bad_vectors1 << endl;
            
            for (int i = 0; i < int(examined.size()); ++i) {
                examined[i] = false;
            }
            num_examined = 0;

            // a round did not result is less differences, restart
            if (last_bad == num_bad_vectors1) {
                ++num_bads;
            } else {
                num_bads = 0;
            }
            if (last_bad == num_bad_vectors1 && num_bads == 2) {
                cout << "randomizing" << endl;
                locked_circuit->randomly_set_keys();
                num_input_patterns += num_tests; 
                num_bads = 0;
                continue;
            } else {
                last_bad = num_bad_vectors1;
            }
        }
        int choose_key;
        do {
            choose_key = rand()%num_keys; 
        } while (examined[choose_key]);
        ++num_examined;
        examined[choose_key] = true;        

        locked_circuit->toggle_key(choose_key);
        simulate_circuits(use_test, use_rand);
        num_input_patterns += num_tests; 
        ++num_iterations;

        int num_bad_outputs2, num_bad_vectors2;
        locked_circuit->output_differences(unlocked_circuit, num_bad_outputs2, num_bad_vectors2); 

        int num_bad_vectors = num_bad_vectors2;
        if (//(num_bad_outputs1 < num_bad_outputs2) ||
              //  ((num_bad_outputs1 == num_bad_outputs2) &&
                 (num_bad_vectors1 <= num_bad_vectors2)) {
            locked_circuit->toggle_key(choose_key);
            //simulate_circuits(use_test, use_rand);
            num_bad_vectors = num_bad_vectors1;
        }

        if (!num_bad_vectors) {
            // use same vecs between
            vector<vector<unsigned long long> > saved_vecs; 
            if (use_rand) {
                saved_vecs = locked_circuit->get_random_inputs();
            }
            locked_circuit->create_random_inputs(VERIFY_LIMIT);
            vector<vector<unsigned long long> > temp_vecs = locked_circuit->get_random_inputs();
            unlocked_circuit->set_random_inputs(temp_vecs, VERIFY_LIMIT);
            locked_circuit->simulate_random();
            unlocked_circuit->simulate_random();

            // check equivalence between circuits up to simulation 
            if (locked_circuit->circuit_sig_equiv(unlocked_circuit)) {
                for (int i = 0; i < num_keys; ++i) {
                    key_values.push_back(locked_circuit->get_current_key_value(i));
                }
                found = true;
                break; 
            } else {
                if (use_rand) {
                    locked_circuit->set_random_inputs(saved_vecs, rand_sim);
                    unlocked_circuit->set_random_inputs(saved_vecs, rand_sim);
                }
                cout << "restarting" << endl;
                ++num_restarts;
                for (int i = 0; i < int(examined.size()); ++i) {
                    examined[i] = false;
                    locked_circuit->randomly_set_keys();
                    num_input_patterns += num_tests; 
                }
            }
        }
    } 

    if (!found) {
        cout << "No key found within " << SEARCH_LIMIT << " simulation vectors" << endl; 
    }

    return found;
}

void CrackKey::simulate_circuits(bool use_test, bool use_rand)
{
    if (use_rand) {
        locked_circuit->simulate_random();
        unlocked_circuit->simulate_random();
        locked_circuit->set_disable_signature_clear(true);
        unlocked_circuit->set_disable_signature_clear(true);
    }
    if (use_test) {
        locked_circuit->simulate_test();
        unlocked_circuit->simulate_test();
    }

    locked_circuit->set_disable_signature_clear(false);
    unlocked_circuit->set_disable_signature_clear(false);
}

void CrackKey::print_info()
{
    cout << "Num search patterns used: " << num_input_patterns << endl;
    cout << "Num keys tried: " << num_iterations << endl;
    cout << "Num restarts: " << num_restarts << endl;
    cout << "Num verification vectors: " << VERIFY_LIMIT << endl;
}
