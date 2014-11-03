add_executable( runBenchmarks
    Benchmark_Construction.cpp
)

# Path to the locally compiled z3 library
#set(z3lib /usr/local/include/z3/build/libz3.so)
#include_directories(/usr/local/include/z3/src/util/)

set(benchmark_libraries ${GTEST_LIBRARIES} lib_carl ${libs_dynamic} ginac)

if (COMPARE_WITH_Z3)
    include_directories(${Z3_INCLUDE_DIRS})
    set(benchmark_libraries ${benchmark_libraries} ${Z3_LIBRARIES})
endif()

target_link_libraries(runBenchmarks ${benchmark_libraries})

# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/tests/benchmarks/config.h.in 
				${CMAKE_SOURCE_DIR}/src/tests/benchmarks/config.h
)  
