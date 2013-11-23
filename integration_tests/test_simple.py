import circuitlock_test_compare

exe_string = '${BUILDLOC}/bin/CircuitLock ${CMAKE_SOURCE_DIR}/integration_tests/inputs/majority.blif'

outfile = "test_simple.out" 
file_comps = [] 

circuitlock_test_compare.compare_outputs(exe_string, outfile, file_comps)
