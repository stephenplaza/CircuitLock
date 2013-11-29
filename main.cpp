#include "EncryptedCircuit.h"
#include "TechLibrary.h"
#include "OptionParser.h"
#include "utils.h"
#include "CrackKey.h"
#include <cstdlib>

using namespace EschewObfuscation;
using std::string;
using std::cout; using std::endl;
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
        parser.parse_options(argc, argv);

        srand(random_seed);

        TechLibrary library;
        EncryptedCircuit circuit(blif_file, &library);
        circuit.print_info();

        if (test_file != "") {
            circuit.load_test_vectors(test_file);
        }

        if (random_xors > 0) {
            circuit.add_random_xors(random_xors);
            circuit.print_keys();
            circuit.print_info();    
        }

        if (random_mux > 0) {
            circuit.add_test_mux(random_mux);
            circuit.print_keys();
            circuit.print_info();

            while (test_rounds-- > 0) {
                circuit.print_testability();
            } 
        }
  
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

        if (crack_key && ((random_xors > 0) || (random_mux > 0))) {
            Circuit unlocked_circuit(blif_file, &library);
            unlocked_circuit.load_test_vectors(test_file);
            CrackKey crack(&unlocked_circuit, &circuit);
            vector<bool> key_values;
            if (crack.generate_key(key_values)) {
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
                crack.print_info();
            } 
        }


        if (output_file != "") {
            circuit.write_blif(output_file);
        }
    } catch (Error &msg) {
        cout << "Error: " << msg.msg << endl;
    }

    return 0;
}














