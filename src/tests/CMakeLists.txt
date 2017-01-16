
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
