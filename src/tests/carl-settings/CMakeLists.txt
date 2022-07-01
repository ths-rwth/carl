file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runSettingsTests ${test_sources})

target_link_libraries(runSettingsTests carl-arith-shared carl-settings-shared ${GTEST_LIBRARIES})

add_test(NAME settings COMMAND runSettingsTests)
add_dependencies(all-tests runSettingsTests)
