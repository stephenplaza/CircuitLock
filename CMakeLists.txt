CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project (CircuitLock)

set (CMAKE_CXX_FLAGS_RELEASE "-O3")
set (CMAKE_CXX_FLAGS_DEBUG "-Wall -ggdb")
set (CMAKE_CXX_LINK_FLAGS "-O3")
set (CMAKE_DEBUG_POSTFIX "-g")

# set the installation to the source bin directory
set (BUILDLOC ${CMAKE_SOURCE_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILDLOC}/bin)
if (NOT EXISTS ${BUILDLOC}/bin)
    file (MAKE_DIRECTORY ${BUILDLOC}/bin)
endif() 

add_executable(CircuitLock TechLibrary.cpp Wire.cpp Inst.cpp Circuit.cpp EncryptedCircuit.cpp CrackKey.cpp main.cpp)
target_link_libraries(CircuitLock boost_system boost_program_options)

# testing requires python
enable_testing()
add_test("test_simple"
    python 
    ${CMAKE_SOURCE_DIR}/integration_tests/test_simple.py
    ${BUILDLOC}
    ${CMAKE_SOURCE_DIR}
)
add_test("test_outcompare"
    python 
    ${CMAKE_SOURCE_DIR}/integration_tests/test_outcompare.py
    ${BUILDLOC}
    ${CMAKE_SOURCE_DIR}
)
add_test("test_simpxor"
    python 
    ${CMAKE_SOURCE_DIR}/integration_tests/test_simpxor.py
    ${BUILDLOC}
    ${CMAKE_SOURCE_DIR}
)
add_test("test_aluxor"
    python 
    ${CMAKE_SOURCE_DIR}/integration_tests/test_aluxor.py
    ${BUILDLOC}
    ${CMAKE_SOURCE_DIR}
)
add_test("test_seqxor"
    python 
    ${CMAKE_SOURCE_DIR}/integration_tests/test_seqxor.py
    ${BUILDLOC}
    ${CMAKE_SOURCE_DIR}
)
