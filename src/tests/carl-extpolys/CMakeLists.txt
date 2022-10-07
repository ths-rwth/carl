file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runExtPolysTests ${test_sources})

target_link_libraries(runExtPolysTests carl-extpolys-shared TestCommon)

add_test(NAME extpolys COMMAND runExtPolysTests)
add_dependencies(all-tests runExtPolysTests)
