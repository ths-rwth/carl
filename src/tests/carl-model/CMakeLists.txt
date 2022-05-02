file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runModelTests ${test_sources})

target_link_libraries(runModelTests TestCommon carl-formula-shared)

add_test(NAME model COMMAND runModelTests)
add_dependencies(all-tests runModelTests)
