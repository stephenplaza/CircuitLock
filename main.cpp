#include "EncryptedCircuit.h"
#include "TechLibrary.h"
#include "OptionParser.h"
#include "utils.h"

using namespace EschewObfuscation;
using std::string;
using std::cout; using std::endl;


int main(int argc, char** argv)
{
    string blif_file;
    string output_file;
    string test_file;
    
    int random_xors = 0;

    try {
        OptionParser parser("Program for obfuscating and cracking a combinational circuit"); 
        parser.add_positional(blif_file, "blif-file", "circuit in BLIF format");
        parser.add_option(output_file, "write-blif", "Write circuit in BLIF format to specified file");
        parser.add_option(random_xors, "lock-randxor", "Number of random XORs to add");
        parser.add_option(test_file, "test-file", "File containing test vectors");
        parser.parse_options(argc, argv);

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

        if (output_file != "") {
            circuit.write_blif(output_file);
        }
    } catch (Error &msg) {
        cout << "Error: " << msg.msg << endl;
    }

    return 0;
}














