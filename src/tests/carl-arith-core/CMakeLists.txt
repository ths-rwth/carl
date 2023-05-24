file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runArithCoreTests ${test_sources})

target_link_libraries(runArithCoreTests TestCommon)

add_test(NAME core COMMAND runArithCoreTests)
add_dependencies(all-tests runArithCoreTests)
