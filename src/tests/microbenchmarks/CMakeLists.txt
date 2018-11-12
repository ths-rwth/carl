file(GLOB_RECURSE test_sources "*.cpp")

add_executable(runMicroBenchmarks EXCLUDE_FROM_ALL ${test_sources})

target_link_libraries(runMicroBenchmarks TestCommon GBCORE_STATIC GBMAIN_STATIC)

if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	message(WARNING "Executing microbenchmarks in debug probably yields wrong results.")
endif()