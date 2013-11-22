#include "TechLibrary.h"

using std::string;

int lib_cell::simulate(string& inputstr)
{
    int output_val;
    int count= 0, match;
    string::iterator pstr;
    int result= 0;

    match= 0;
    for (pstr= blif.begin(); pstr != blif.end(); pstr++) {
	if (count < num_inputs) {
	    if (*pstr == '-' || inputstr[count] == *pstr) {
		match++;
            }
	    if (match == num_inputs) {
		result= 1;
	    }
	    count++;
	}
	else if (count >= num_inputs && (*pstr == '0' || *pstr == '1')) {
	    output_val= (*pstr == '0') ? 0 : 1;
	}

	if (*pstr == '\n') {
	    if (result == 1) {
                break;
            }
	    count= 0;
	    match= 0;
	}
    }
    if (result == 1) {
	return output_val;
    }
	
    return 1 - output_val;
}

lib_cell* TechLibrary::create_libcell(string blif, int num_inputs)
{
    lib_cell lcell(num_inputs, blif);
    lib_cell* nlcell;

    Library_t::iterator pset= cells.find(&lcell);
    if (pset == cells.end()) {
        if (num_inputs <= 8) {
            // Now input_no <= 8, need to calculate truth table first
            // and use it to find ncell
            int num_minterms = 1 << num_inputs;
            for (int minterm= 0; minterm < num_minterms; ++minterm) {
                string minstr;
                for (int i = num_inputs - 1; i >= 0; --i) {
                    if (minterm & (1 << i))
                        minstr += '1';
                    else
                        minstr += '0';
                }
                int result = lcell.simulate(minstr);
                lcell.set_minterm(minterm, result);
            }
        }

        nlcell= new lib_cell(lcell);
        cells.insert(nlcell);
    } else {
        nlcell= *pset;
    }

    return nlcell;
}

TechLibrary::~TechLibrary()
{
    for (Library_t::iterator iter = cells.begin(); iter != cells.end(); ++iter) {
        delete *iter;
    }
}


