include_directories(${GTEST_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/src ${include_dirs})

add_library(prepareTest Common.cpp)

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
# Only for debugging.
#add_subdirectory(debug)
