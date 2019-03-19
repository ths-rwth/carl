file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runCoveringTests ${test_sources})

target_link_libraries(runCoveringTests carl-covering-shared ${GTEST_LIBRARIES})

add_test(NAME covering COMMAND runCoveringTests)
add_dependencies(all-tests runCoveringTests)
