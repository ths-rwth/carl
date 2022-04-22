file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runPolyTests ${test_sources})

target_link_libraries(runPolyTests TestCommon carl-model-shared)

add_test( NAME formula COMMAND runPolyTests )
add_dependencies(all-tests runPolyTests)
