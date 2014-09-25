# Add test cpp file
add_executable( runDebugTests
    Test_GCC.cpp
)
target_link_libraries(runDebugTests ${GTEST_LIBRARIES} lib_carl ${libs_dynamic})

add_test( NAME debug COMMAND runDebugTests )