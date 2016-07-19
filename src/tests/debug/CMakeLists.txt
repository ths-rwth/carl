# Add test cpp file
add_executable( runDebugTests
    Test_GCC.cpp
)
target_link_libraries(runDebugTests TestCommon)

add_test( NAME debug COMMAND runDebugTests )
