#ifndef TECHLIBRARY_H 
#define TECHLIBRARY_H

#include <bitset>
#include <string>
#include <tr1/unordered_set>
#include <boost/functional/hash.hpp>

// currently, all cells with > 8 inputs does not use truth table and still
// uses blif
class lib_cell {
  public:
    lib_cell(int num_inputs_, std::string blif_) : 
        num_inputs(num_inputs_), blif(blif_) {}
      
    /*!
     * Simulate output given input given by string of 0/1s
    */
    int simulate(std::string& inputstr);


    int get_minterm(int row)
    {
        return ttable[row];
    }

    void set_minterm(int row, int result)
    {
        ttable[row] = result;
    }

    unsigned long get_table_value() const
    {
        return ttable.to_ulong();
    } 

    std::string get_blif() const
    {
        return blif;
    }

  private:
    int num_inputs;  // Number of inputs
    std::bitset<256> ttable; // Truth table, for <= 8 inputs only
    std::string blif; // blif ttable representation
};

struct lib_cell_hash {
    size_t operator()(const lib_cell* cell) const
    {
        return boost::hash_value(cell->get_blif()); 
    }
};

// ?! allow blif to be build from ttable ?? (in old circuit)

/*!
 * Collection of cells containing truth tables for simulation.
*/
class TechLibrary {
  public:
    /*!
     * Create a library cell and add it to the heap  and return
     * the cell (or return a previously created library cell)
    */
    lib_cell* create_libcell(std::string blif, int num_inputs);

    ~TechLibrary();

  private:
    //! each cell has only one output
    typedef std::tr1::unordered_set<lib_cell*, lib_cell_hash> Library_t;
    Library_t cells;
};


#endif

