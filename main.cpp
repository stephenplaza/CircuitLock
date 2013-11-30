#include "EncryptedCircuit.h"
#include "TechLibrary.h"
#include "OptionParser.h"
#include "utils.h"
#include "CrackKey.h"
#include <cstdlib>
#include "Inst.h"

using namespace EschewObfuscation;
using std::string;
using std::cout; using std::endl;
using std::vector;
using std::tr1::unordered_set;
using std::vector;

int main(int argc, char** argv)
{
    string blif_file;
    string output_file;
    string test_file;
    int random_xors = 0;
    bool crack_key = false;
    int random_seed = 0;
    int random_mux = 0;
    int test_rounds = 0;
    bool mux_cands = false;
    bool compute_testability = false;


    try {
        OptionParser parser("Program for obfuscating and cracking a combinational circuit"); 
        parser.add_positional(blif_file, "blif-file", "circuit in BLIF format");
        parser.add_option(output_file, "write-blif", "Write COMBINATIONAL circuit in BLIF format to specified file (will remove latches from sequential circuit)");
        parser.add_option(random_xors, "lock-randxor", "Number of random XORs to add");
        parser.add_option(random_mux, "lock-mux", "Number of random test-aware MUXs to add");
        parser.add_option(test_file, "test-file", "File containing test vectors");
        parser.add_option(crack_key, "crack-key", "Try to crack the key"); 
        parser.add_option(test_rounds, "num-test-rounds", "Number of rounds of testing on mux locked circuit"); 
        parser.add_option(random_seed, "random-seed", "Initial seed to use for execution");    
        parser.add_option(mux_cands, "mux-cands", "Show random MUX candidates", true, false, true); 
        parser.add_option(compute_testability, "compute-testability", "Compute testability of original circuit"); 
        parser.parse_options(argc, argv);

        srand(random_seed);

        cout << "Read Circuit" << endl;
        TechLibrary library;
        EncryptedCircuit circuit(blif_file, &library);
        circuit.print_info();

        if (test_file != "") {
            circuit.load_test_vectors(test_file);
        }

        if (compute_testability) {
            circuit.print_testability();            
        }

        if (random_xors > 0) {
            cout << "Add XORs" << endl;
            circuit.add_random_xors(random_xors);
            circuit.print_keys();
            circuit.print_info();    
        }

        if (random_mux > 0) {
            cout << "Add MUXs" << endl;
            circuit.add_test_mux(random_mux, mux_cands);
            circuit.print_keys();
            circuit.print_info();
        }

/*
        Circuit unlocked_circuit(blif_file, &library);
        unlocked_circuit.load_test_vectors(test_file);
        circuit.correctly_set_keys();
        circuit.create_random_inputs(1024);
        vector<vector<unsigned long long> > rand_vecs = circuit.get_random_inputs();
        unlocked_circuit.set_random_inputs(rand_vecs, 1024);
        circuit.simulate_random();
        unlocked_circuit.simulate_random();
        int num_bad_outputs1, num_bad_vectors1;
        circuit.output_differences(&unlocked_circuit, num_bad_outputs1, num_bad_vectors1);
        cout << "Num diffs: " << num_bad_outputs1 << " " << num_bad_vectors1 << endl;

        circuit.randomly_set_keys();
        circuit.simulate_random();
        unlocked_circuit.simulate_random();
        num_bad_outputs1, num_bad_vectors1;
        circuit.output_differences(&unlocked_circuit, num_bad_outputs1, num_bad_vectors1);
        cout << "Num diffs2: " << num_bad_outputs1 << " " << num_bad_vectors1 << endl;

        circuit.simulate_test();
        unlocked_circuit.simulate_test();
        num_bad_outputs1, num_bad_vectors1;
        circuit.output_differences(&unlocked_circuit, num_bad_outputs1, num_bad_vectors1);
        cout << "Num diffs3: " << num_bad_outputs1 << " " << num_bad_vectors1 << endl;

        exit(-1);
*/
 
        /* 
        {
            circuit.randomly_set_keys();
            Circuit unlocked_circuit(blif_file, &library);
            unlocked_circuit.load_test_vectors(test_file);
            
            circuit.create_random_inputs(64);
            vector<vector<unsigned long long> > rand_vecs = circuit.get_random_inputs();
            unlocked_circuit.set_random_inputs(rand_vecs, 64);
            circuit.simulate_test();
            unlocked_circuit.simulate_test();
            int num_bad_outputs1, num_bad_vectors1;
            circuit.output_differences(&unlocked_circuit, num_bad_outputs1, num_bad_vectors1);
            cout << "Num diffs: " << num_bad_outputs1 << " " << num_bad_vectors1 << endl;

            for (int i = 0; i < 64; ++i) {
                circuit.toggle_key(i);
                circuit.simulate_test();
                unlocked_circuit.simulate_test();
                int num_bad_outputs1, num_bad_vectors1;
                circuit.output_differences(&unlocked_circuit, num_bad_outputs1, num_bad_vectors1);
                cout << "Toggle: " << i << " " <<  num_bad_outputs1 << " " << num_bad_vectors1 << endl;
                circuit.toggle_key(i);
             }

            exit(1);
        }
        */
    
        unordered_set<Inst*> stuck0;
        unordered_set<Inst*> stuck1;
        if ((test_rounds > 0) && (random_mux > 0)) {
            cout << "Compute fault percentage" << endl;
            vector<Inst*> new_gates = circuit.get_new_gates();
            for (int i = 0; i < new_gates.size(); ++i) {
                new_gates[i]->set_visited(true);
            }

            circuit.correctly_set_keys();
            circuit.print_testability();            
            
            while (test_rounds-- > 0) { 
                circuit.randomly_set_keys();
                circuit.print_testability_prob(stuck0, stuck1);
            }

            for (int i = 0; i < new_gates.size(); ++i) {
                new_gates[i]->set_visited(false);
            }
        }

        if (crack_key && ((random_xors > 0) || (random_mux > 0))) {
            cout << "Crack the keys" << endl;
            bool use_test = true;
            int rand_sim = 0;
            if (random_mux > 0) {
                rand_sim = 1024;
                use_test = false;
            }

            Circuit unlocked_circuit(blif_file, &library);
            unlocked_circuit.load_test_vectors(test_file);
            CrackKey crack(&unlocked_circuit, &circuit);
            vector<bool> key_values;
            if (crack.generate_key(key_values, rand_sim, use_test)) {
                bool equal = true;  
                for (int i = 0; i < int(key_values.size()); ++i) {
                    if (key_values[i] != circuit.get_key_value(i)) {
                        equal = false;
                        cout << "Did not find correct key" << endl;
                        break;
                    }
                } 
                if (equal) {
                    cout << "Found correct key" << endl;
                }
            } 
            crack.print_info();
        }


        if (output_file != "") {
            circuit.write_blif(output_file);
        }
    } catch (Error &msg) {
        cout << "Error: " << msg.msg << endl;
    }

    return 0;
}














