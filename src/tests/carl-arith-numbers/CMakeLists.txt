file(GLOB_RECURSE test_sources "*.cpp")
add_executable(runArithNumbersTests ${test_sources})

target_link_libraries(runArithNumbersTests TestCommon)

add_test(NAME numbers COMMAND runArithNumbersTests)
add_dependencies(all-tests runArithNumbersTests)
