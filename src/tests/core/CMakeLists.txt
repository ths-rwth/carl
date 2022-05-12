file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runCoreTests ${test_sources})

target_link_libraries(runCoreTests TestCommon)

add_test(NAME core COMMAND runCoreTests)
add_dependencies(all-tests runCoreTests)
