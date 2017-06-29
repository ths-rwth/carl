ExternalProject_Add(
    GTest_EP
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG "release-${GTEST_VERSION}"
	UPDATE_COMMAND ""
	BUILD_COMMAND cmake --build . --config RELEASE --target gtest
	COMMAND cmake --build . --config RELEASE --target gtest_main
	INSTALL_COMMAND ""
	BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/resources/src/GTest_EP-build/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${STATIC_EXT} ${CMAKE_BINARY_DIR}/resources/src/GTest_EP-build/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${STATIC_EXT}
)

ExternalProject_Get_Property(GTest_EP source_dir)
ExternalProject_Get_Property(GTest_EP binary_dir)

if(WIN32)
	add_imported_library(GTESTCORE STATIC "${binary_dir}/googlemock/gtest/Release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${STATIC_EXT}" "${source_dir}/googletest/include")
	add_imported_library(GTESTMAIN STATIC "${binary_dir}/googlemock/gtest/Release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${STATIC_EXT}" "${source_dir}/googletest/include")

	set(GTEST_LIBRARIES GTESTCORE_STATIC GTESTMAIN_STATIC)
else()
	add_imported_library(GTESTCORE STATIC "${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${STATIC_EXT}" "${source_dir}/googletest/include")
	add_imported_library(GTESTMAIN STATIC "${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${STATIC_EXT}" "${source_dir}/googletest/include")

	set(GTEST_LIBRARIES GTESTCORE_STATIC GTESTMAIN_STATIC pthread dl)
endif()

add_dependencies(GTESTCORE_STATIC GTest_EP)
add_dependencies(GTESTMAIN_STATIC GTest_EP)
