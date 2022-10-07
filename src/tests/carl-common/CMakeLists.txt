file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runCommonTests ${test_sources})

target_link_libraries(runCommonTests carl-common-shared TestCommon)

add_test(NAME common COMMAND runCommonTests)
add_dependencies(all-tests runCommonTests)
