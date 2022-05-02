file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runIntervalContractionTests ${test_sources})

if (COMPARE_WITH_Z3)
	include_directories(${Z3_INCLUDE_DIRS})
	target_link_libraries(runIntervalContractionTests ${Z3_LIBRARIES})
endif()

target_link_libraries(runIntervalContractionTests TestCommon)

add_test( NAME intervalcontraction COMMAND runIntervalContractionTests )
add_dependencies(all-tests runIntervalContractionTests)
