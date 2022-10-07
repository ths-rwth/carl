
add_custom_target(all-tests)

add_library(TestCommon STATIC Common.cpp)
target_link_libraries(TestCommon carl-arith-shared GTESTCORE_STATIC GTESTMAIN_STATIC pthread dl)
add_dependencies(TestCommon resources GTESTCORE_STATIC GTESTMAIN_STATIC)


################################
# Unit Tests
################################
add_subdirectory(carl-covering)
add_subdirectory(carl-extpolys)
add_subdirectory(carl-io)
add_subdirectory(carl-common)
add_subdirectory(carl-settings)
add_subdirectory(carl-statistics)
add_subdirectory(carl-logging)
add_subdirectory(numbers)
add_subdirectory(core)
add_subdirectory(poly)
add_subdirectory(ran)
add_subdirectory(carl-formula)
add_subdirectory(groebner)
add_subdirectory(interval)
add_subdirectory(intervalcontraction)
add_subdirectory(benchmarks)
add_subdirectory(pycarl)

# Only for debugging.
#add_subdirectory(debug)

add_subdirectory(microbenchmarks)

clang_tidy_recurse("${CMAKE_SOURCE_DIR}/src/tests" "test")
