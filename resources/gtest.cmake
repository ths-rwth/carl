ExternalProject_Add(
    googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG "release-${GTEST_VERSION}"
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(googletest source_dir)
ExternalProject_Get_Property(googletest binary_dir)

add_imported_library(GTESTCORE STATIC "${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a" "${source_dir}/include")
add_imported_library(GTESTMAIN STATIC "${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a" "${source_dir}/include")

set(GTEST_LIBRARIES GTESTCORE_STATIC GTESTMAIN_STATIC pthread dl)

add_dependencies(resources googletest)
