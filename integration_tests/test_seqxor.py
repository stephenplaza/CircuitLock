import circuitlock_test_compare

exe_string = '${BUILDLOC}/bin/CircuitLock ${CMAKE_SOURCE_DIR}/integration_tests/inputs/sasc.blif --lock-randxor 64 --write-blif ${CMAKE_SOURCE_DIR}/integration_tests/temp_data/sasc.blif'

outfile = "test_seqxor.out" 
file_comps = ["sasc.blif"] 

circuitlock_test_compare.compare_outputs(exe_string, outfile, file_comps)
