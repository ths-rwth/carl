file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runIntervalTests ${test_sources})

if (COMPARE_WITH_Z3)
	include_directories(${Z3_INCLUDE_DIRS})
	target_link_libraries(runIntervalTests ${Z3_LIBRARIES})
endif()

target_link_libraries(runIntervalTests TestCommon)

add_test( NAME interval COMMAND runIntervalTests )
