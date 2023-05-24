file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runArithPolyTests ${test_sources})

target_link_libraries(runArithPolyTests TestCommon)

add_test( NAME poly COMMAND runArithPolyTests )
add_dependencies(all-tests runArithPolyTests)
