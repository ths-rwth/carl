file(GLOB_RECURSE test_sources "*.cpp")
add_executable(runNumbersTests ${test_sources})

target_link_libraries(runNumbersTests TestCommon)

add_test(NAME numbers COMMAND runNumbersTests)
add_dependencies(all-tests runNumbersTests)
