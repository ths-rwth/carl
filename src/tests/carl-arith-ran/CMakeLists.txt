file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runArithRanTests ${test_sources})

target_link_libraries(runArithRanTests TestCommon)

add_test(NAME ran COMMAND runArithRanTests)
add_dependencies(all-tests runArithRanTests)
