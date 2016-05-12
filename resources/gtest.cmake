ExternalProject_Add(
    googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG "release-${GTEST_VERSION}"
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIR "${source_dir}/include")

ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBRARIES "${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a;${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a")

add_dependencies(resources googletest)
