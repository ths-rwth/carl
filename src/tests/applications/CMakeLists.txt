file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runApplicationTests ${test_sources})

target_link_libraries(runApplicationTests TestCommon)

add_test(NAME applications COMMAND runApplicationTests)
add_dependencies(all-tests runApplicationTests)
