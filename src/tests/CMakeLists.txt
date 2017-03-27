
#include_directories(${CMAKE_SOURCE_DIR}/src ${include_dirs})

add_library(TestCommon SHARED Common.cpp)
target_link_libraries(TestCommon lib_carl ${GTEST_LIBRARIES})
add_dependencies(TestCommon resources lib_carl GTESTCORE_STATIC GTESTMAIN_STATIC)

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

if (CLANG_TIDY)
	add_custom_target(tidy-tests
		COMMAND clang-tidy -p ../../build/compile_commands.json ${CLANG_TIDY_CHECKS} -header-filter='.*' ${carl_lib_sources}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/carl/
	)
	file(GLOB dirs LIST_DIRECTORIES true RELATIVE "${CMAKE_SOURCE_DIR}/src/tests/" "*")
	foreach(dir ${dirs})
		if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/src/tests/${dir}")
			file(GLOB_RECURSE sources_${dir} "${CMAKE_SOURCE_DIR}/src/tests/${dir}/*.cpp")
			add_custom_target(tidy-tests-${dir}
				COMMAND clang-tidy -p ../../build/compile_commands.json ${CLANG_TIDY_CHECKS} -header-filter='.*' ${sources_${dir}}
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/tests/
			)
		endif()
	endforeach()
endif()
