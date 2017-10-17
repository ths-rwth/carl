file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runFormulaTests ${test_sources})

target_link_libraries(runFormulaTests TestCommon)

add_test( NAME formula COMMAND runFormulaTests )
add_dependencies(all-tests runFormulaTests)
