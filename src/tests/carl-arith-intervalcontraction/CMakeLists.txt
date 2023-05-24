file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runArithIntervalContractionTests ${test_sources})

target_link_libraries(runArithIntervalContractionTests TestCommon)

add_test( NAME intervalcontraction COMMAND runArithIntervalContractionTests )
add_dependencies(all-tests runArithIntervalContractionTests)
