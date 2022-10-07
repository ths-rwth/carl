file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runStatisticsTests ${test_sources})

target_link_libraries(runStatisticsTests carl-arith-shared carl-statistics-shared TestCommon)

add_test(NAME settings COMMAND runStatisticsTests)
add_dependencies(all-tests runStatisticsTests)
