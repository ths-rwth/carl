file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runIOTests ${test_sources})

target_link_libraries(runIOTests TestCommon carl-io-shared)

add_test(NAME io COMMAND runIOTests)
add_dependencies(all-tests runIOTests)
