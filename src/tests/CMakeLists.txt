
add_custom_target(all-tests)

add_library(TestCommon STATIC Common.cpp)
if(COMPARE_WITH_Z3 OR RAN_USE_Z3 OR USE_Z3_NUMBERS)
	target_link_libraries(TestCommon carl-shared ${GTEST_LIBRARIES} Z3_STATIC gomp)
else()
	target_link_libraries(TestCommon carl-shared ${GTEST_LIBRARIES})
endif()
add_dependencies(TestCommon resources GTESTCORE_STATIC GTESTMAIN_STATIC)

################################
# Unit Tests
################################
add_subdirectory(util)
add_subdirectory(carl-cad)
add_subdirectory(carl-covering)
add_subdirectory(carl-extpolys)
add_subdirectory(carl-io)
add_subdirectory(carl-model)
add_subdirectory(carl-settings)
add_subdirectory(numbers)
#add_subdirectory(numbers_boost)
add_subdirectory(core)
add_subdirectory(formula)
add_subdirectory(groebner)
add_subdirectory(interval)
add_subdirectory(benchmarks)
add_subdirectory(pycarl)
#add_subdirectory(jcarl)
# Only for debugging.
#add_subdirectory(debug)

add_subdirectory(microbenchmarks)

clang_tidy_recurse("${CMAKE_SOURCE_DIR}/src/tests" "test")
