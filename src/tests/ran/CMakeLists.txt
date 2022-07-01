file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runRanTests ${test_sources})

target_link_libraries(runRanTests TestCommon)

add_test(NAME ran COMMAND runRanTests)
add_dependencies(all-tests runRanTests)
