file(GLOB_RECURSE test_sources "*.cpp")
add_executable(runUtilTests ${test_sources})

target_link_libraries(runUtilTests TestCommon)
add_test(NAME util COMMAND runUtilTests)
