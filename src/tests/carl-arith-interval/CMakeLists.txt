file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runArithIntervalTests ${test_sources})

target_link_libraries(runArithIntervalTests TestCommon)

add_test( NAME interval COMMAND runArithIntervalTests )
add_dependencies(all-tests runArithIntervalTests)
