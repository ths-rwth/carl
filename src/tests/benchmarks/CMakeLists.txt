add_executable( runBenchmarks
    Benchmark_Construction.cpp
)

# Path to the locally compiled z3 library
#set(z3lib /usr/local/include/z3/build/libz3.so)
#include_directories(/usr/local/include/z3/src/util/)

#if (COMPARE_WITH_Z3)
#    include_directories(${Z3_INCLUDE_DIRS})
#    target_link_libraries(runBenchmarks ${Z3_LIBRARIES})
#endif()

target_link_libraries(runBenchmarks TestCommon)

# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/tests/benchmarks/config.h.in 
				${CMAKE_SOURCE_DIR}/src/tests/benchmarks/config.h
)  
