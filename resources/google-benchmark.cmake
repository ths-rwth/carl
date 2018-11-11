ExternalProject_Add(
	google-benchmark-EP
	GIT_REPOSITORY https://github.com/google/benchmark.git
	GIT_TAG "v1.4.1"
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=RELEASE -DBENCHMARK_DOWNLOAD_DEPENDENCIES=ON
	UPDATE_COMMAND ""
)

ExternalProject_Get_Property(google-benchmark-EP install_dir)

add_imported_library(GBCORE STATIC "${install_dir}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark${STATIC_EXT}" "${install_dir}/include")
add_imported_library(GBMAIN STATIC "${install_dir}/lib/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark_main${STATIC_EXT}" "${install_dir}/include")


add_dependencies(GBCORE_STATIC google-benchmark-EP)
add_dependencies(GBMAIN_STATIC google-benchmark-EP)

add_dependencies(resources google-benchmark-EP)