add_executable( runBenchmarks
    Benchmark_Construction.cpp
)

# Path to the locally compiled z3 library
set(z3lib /usr/local/lib/libz3.dylib)
include_directories(/usr/local/include/z3/src/util/)

target_link_libraries(runBenchmarks ${GTEST_LIBRARIES} lib_carl ${libs_dynamic} ginac ${z3lib})

# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/tests/benchmarks/config.h.in 
				${CMAKE_SOURCE_DIR}/src/tests/benchmarks/config.h
)  
