add_executable( runBenchmarks
    Benchmark_Construction.cpp
)

target_link_libraries(runBenchmarks TestCommon)

# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/tests/benchmarks/config.h.in 
				${CMAKE_SOURCE_DIR}/src/tests/benchmarks/config.h
)  
