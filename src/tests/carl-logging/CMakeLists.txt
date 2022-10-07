file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runLoggingTests ${test_sources})

target_link_libraries(runLoggingTests carl-logging-shared TestCommon)

add_test(NAME logging COMMAND runLoggingTests)
add_dependencies(all-tests runLoggingTests)
