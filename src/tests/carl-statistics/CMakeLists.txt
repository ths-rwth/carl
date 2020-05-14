file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runStatisticsTests ${test_sources})

target_link_libraries(runStatisticsTests carl-shared carl-statistics-shared ${GTEST_LIBRARIES})

add_test(NAME settings COMMAND runStatisticsTests)
add_dependencies(all-tests runStatisticsTests)
