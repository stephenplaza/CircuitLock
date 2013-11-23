import circuitlock_test_compare

exe_string = '${BUILDLOC}/bin/CircuitLock ${CMAKE_SOURCE_DIR}/integration_tests/inputs/majority.blif --write-blif ${CMAKE_SOURCE_DIR}/integration_tests/temp_data/majority.blif'

outfile = "test_outcompare.out" 
file_comps = ["majority.blif"] 

circuitlock_test_compare.compare_outputs(exe_string, outfile, file_comps)
