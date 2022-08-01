file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runPolyTests ${test_sources})

target_link_libraries(runPolyTests TestCommon)

add_test( NAME poly COMMAND runPolyTests )
add_dependencies(all-tests runPolyTests)
