
add_custom_target(all-tests)

add_library(TestCommon STATIC Common.cpp)
target_link_libraries(TestCommon carl-shared ${GTEST_LIBRARIES} Z3_STATIC gomp)
add_dependencies(TestCommon resources GTESTCORE_STATIC GTESTMAIN_STATIC)

################################
# Unit Tests
################################
add_subdirectory(util)
add_subdirectory(numbers)
#add_subdirectory(numbers_boost)
add_subdirectory(core)
add_subdirectory(formula)
add_subdirectory(cad)
add_subdirectory(groebner)
add_subdirectory(interval)
add_subdirectory(benchmarks)
add_subdirectory(pycarl)
#add_subdirectory(jcarl)
# Only for debugging.
#add_subdirectory(debug)

add_subdirectory(microbenchmarks)

if (CLANG_TIDY)
	add_custom_target(tidy-tests
		COMMAND ${CLANG_TIDY} -p ../../build/compile_commands.json ${CLANG_TIDY_CHECKS} -header-filter='.*' ${carl_lib_sources}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/carl/
	)
	file(GLOB dirs LIST_DIRECTORIES true RELATIVE "${CMAKE_SOURCE_DIR}/src/tests/" "*")
	foreach(dir ${dirs})
		if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/src/tests/${dir}")
			file(GLOB_RECURSE sources_${dir} "${CMAKE_SOURCE_DIR}/src/tests/${dir}/*.cpp")
			add_custom_target(tidy-tests-${dir}
				COMMAND ${CLANG_TIDY} -p ../../build/compile_commands.json ${CLANG_TIDY_CHECKS} -header-filter='.*' ${sources_${dir}}
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/tests/
			)
		endif()
	endforeach()
endif()
