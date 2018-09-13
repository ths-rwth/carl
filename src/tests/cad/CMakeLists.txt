file(GLOB_RECURSE test_sources "*.cpp")
add_executable(runCADTests ${test_sources})

target_link_libraries(runCADTests TestCommon)
add_test(NAME cad COMMAND runCADTests)
add_dependencies(all-tests runCADTests)
