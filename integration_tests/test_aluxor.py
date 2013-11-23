import circuitlock_test_compare

exe_string = '${BUILDLOC}/bin/CircuitLock ${CMAKE_SOURCE_DIR}/integration_tests/inputs/alu4.blif --lock-randxor 64 --write-blif ${CMAKE_SOURCE_DIR}/integration_tests/temp_data/alu4.blif'

outfile = "test_aluxor.out" 
file_comps = ["alu4.blif"] 

circuitlock_test_compare.compare_outputs(exe_string, outfile, file_comps)
