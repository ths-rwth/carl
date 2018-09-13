file(GLOB_RECURSE test_sources "*.cpp")
add_executable(runGroebnerTests ${test_sources})

target_link_libraries(runGroebnerTests TestCommon)
add_test(NAME groebner COMMAND runGroebnerTests)
add_dependencies(all-tests runGroebnerTests)
