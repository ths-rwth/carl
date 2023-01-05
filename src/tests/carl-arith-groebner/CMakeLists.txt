file(GLOB_RECURSE test_sources "*.cpp")
add_executable(runArithGroebnerTests ${test_sources})

target_link_libraries(runArithGroebnerTests TestCommon)
add_test(NAME groebner COMMAND runArithGroebnerTests)
add_dependencies(all-tests runArithGroebnerTests)
