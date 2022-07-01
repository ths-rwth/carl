file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runIntervalTests ${test_sources})

target_link_libraries(runIntervalTests TestCommon)

add_test( NAME interval COMMAND runIntervalTests )
add_dependencies(all-tests runIntervalTests)
