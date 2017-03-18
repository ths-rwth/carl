ExternalProject_Add(
    GTest_EP
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG "release-${GTEST_VERSION}"
	UPDATE_COMMAND ""
	BUILD_COMMAND make gtest gtest_maing
	INSTALL_COMMAND ""
	BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/resources/src/GTest_EP-build/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a ${CMAKE_BINARY_DIR}/resources/src/GTest_EP-build/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a
)

ExternalProject_Get_Property(GTest_EP source_dir)
ExternalProject_Get_Property(GTest_EP binary_dir)

add_imported_library(GTESTCORE STATIC "${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${STATIC_EXT}" "${source_dir}/googletest/include")
add_imported_library(GTESTMAIN STATIC "${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${STATIC_EXT}" "${source_dir}/googletest/include")

set(GTEST_LIBRARIES GTESTCORE_STATIC GTESTMAIN_STATIC pthread dl)

add_dependencies(GTESTCORE_STATIC GTest_EP)
add_dependencies(GTESTMAIN_STATIC GTest_EP)
