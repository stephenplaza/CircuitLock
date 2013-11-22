#include "Circuit.h"
#include "TechLibrary.h"
#include "OptionParser.h"

using namespace EschewObfuscation;
using std::string;
using std::cout; using std::endl;


int main(int argc, char** argv)
{
    string blif_file;
    string output_file;
    
    OptionParser parser("Program for obfuscating and cracking a combinational circuit"); 
    parser.add_positional(blif_file, "blif-file", "circuit in BLIF format");
    parser.add_option(output_file, "write-blif", "Write circuit in BLIF format to specified file");
    parser.parse_options(argc, argv);

    TechLibrary library;
    Circuit circuit(blif_file, &library);
    circuit.print_info();

    if (output_file != "") {
        circuit.write_blif(output_file);
    }

    return 0;
}














