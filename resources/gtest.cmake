ExternalProject_Add(
    GTest-EP
	URL https://github.com/google/googletest/archive/release-${GTEST_VERSION}.zip
	URL_MD5 ${GTEST_ZIPHASH}
	DOWNLOAD_NO_PROGRESS 1
	UPDATE_COMMAND ""
	BUILD_COMMAND cmake --build . --config ${CMAKE_BUILD_TYPE} --target gtest
	COMMAND cmake --build . --config ${CMAKE_BUILD_TYPE} --target gtest_main
	INSTALL_COMMAND ""
	BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/resources/src/GTest-EP-build/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${STATIC_EXT} ${CMAKE_BINARY_DIR}/resources/src/GTest-EP-build/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${STATIC_EXT}
)

ExternalProject_Get_Property(GTest-EP source_dir)
ExternalProject_Get_Property(GTest-EP binary_dir)

if(WIN32)
	add_imported_library(GTESTCORE STATIC "${binary_dir}/googlemock/gtest/${CMAKE_BUILD_TYPE}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${STATIC_EXT}" "${source_dir}/googletest/include")
	add_imported_library(GTESTMAIN STATIC "${binary_dir}/googlemock/gtest/${CMAKE_BUILD_TYPE}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${STATIC_EXT}" "${source_dir}/googletest/include")

	set(GTEST_LIBRARIES GTESTCORE_STATIC GTESTMAIN_STATIC)
else()
	add_imported_library(GTESTCORE STATIC "${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${STATIC_EXT}" "${source_dir}/googletest/include")
	add_imported_library(GTESTMAIN STATIC "${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main${STATIC_EXT}" "${source_dir}/googletest/include")

	set(GTEST_LIBRARIES GTESTCORE_STATIC GTESTMAIN_STATIC pthread dl)
endif()

add_dependencies(GTESTCORE_STATIC GTest-EP)
add_dependencies(GTESTMAIN_STATIC GTest-EP)
