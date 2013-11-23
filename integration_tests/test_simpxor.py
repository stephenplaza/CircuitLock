import circuitlock_test_compare

exe_string = '${BUILDLOC}/bin/CircuitLock ${CMAKE_SOURCE_DIR}/integration_tests/inputs/majority.blif --lock-randxor 1 --write-blif ${CMAKE_SOURCE_DIR}/integration_tests/temp_data/majorityxor.blif'

outfile = "test_simpxor.out" 
file_comps = ["majorityxor.blif"] 

circuitlock_test_compare.compare_outputs(exe_string, outfile, file_comps)
