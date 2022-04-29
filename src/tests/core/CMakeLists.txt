file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runCoreTests ${test_sources})

if (COMPARE_WITH_Z3)
	include_directories(${Z3_INCLUDE_DIRS})
	target_link_libraries(runCoreTests ${Z3_LIBRARIES})
endif()

target_link_libraries(runCoreTests TestCommon carl-formula-shared)

add_test(NAME core COMMAND runCoreTests)
add_dependencies(all-tests runCoreTests)
