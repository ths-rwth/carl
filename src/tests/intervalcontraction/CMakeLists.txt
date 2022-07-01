file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runIntervalContractionTests ${test_sources})

target_link_libraries(runIntervalContractionTests TestCommon)

add_test( NAME intervalcontraction COMMAND runIntervalContractionTests )
add_dependencies(all-tests runIntervalContractionTests)
