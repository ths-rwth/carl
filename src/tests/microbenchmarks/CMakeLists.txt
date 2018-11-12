file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runMicroBenchmarks EXCLUDE_FROM_ALL ${test_sources})

target_link_libraries(runMicroBenchmarks TestCommon GBCORE_STATIC GBMAIN_STATIC)

add_test(NAME microbenchmarks COMMAND runMicroBenchmarks)
