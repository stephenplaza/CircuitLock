CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project (CircuitLock)

set (CMAKE_CXX_FLAGS_RELEASE "-O3")
set (CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set (CMAKE_CXX_LINK_FLAGS "-O3")
set (CMAKE_DEBUG_POSTFIX "-g")

add_executable(CircuitLock TechLibrary.cpp Wire.cpp Inst.cpp Circuit.cpp main.cpp)
target_link_libraries(CircuitLock boost_system boost_program_options)

