file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runMicroBenchmarks ${test_sources})

target_link_libraries(runMicroBenchmarks TestCommon GBCORE_STATIC GBMAIN_STATIC)

add_test(NAME microbenchmarks COMMAND runMicroBenchmarks)
add_dependencies(all-tests runMicroBenchmarks)
