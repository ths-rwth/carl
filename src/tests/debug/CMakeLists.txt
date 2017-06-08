file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runDebugTests ${test_sources})

target_link_libraries(runDebugTests TestCommon)

add_test(NAME debug COMMAND runDebugTests)
