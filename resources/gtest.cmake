ExternalProject_Add(
    GTest-EP
	URL https://github.com/google/googletest/archive/release-${GTEST_VERSION}.zip
	URL_HASH SHA1=${GTEST_ZIPHASH}
	DOWNLOAD_NO_PROGRESS 1
	CMAKE_ARGS
          -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	BUILD_COMMAND cmake --build . --config ${CMAKE_BUILD_TYPE} --target gtest
	COMMAND cmake --build . --config ${CMAKE_BUILD_TYPE} --target gtest_main
	#INSTALL_COMMAND ""
	BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libgtest.a <INSTALL_DIR>/lib/libgtest_main.a
)

set(GTEST_INCLUDE_DIR "${CMAKE_BINARY_DIR}/resources/include")
set(GTEST_LIB "${CMAKE_BINARY_DIR}/resources/src/GTest-EP-build/lib/libgtest.a")
set(GTEST_MAIN_LIBRARIES "${CMAKE_BINARY_DIR}/resources/src/GTest-EP-build/lib/libgtest_main.a")

add_library(GTESTCORE_STATIC STATIC IMPORTED)
set_target_properties(GTESTCORE_STATIC PROPERTIES
	IMPORTED_LOCATION "${GTEST_LIB}"
	INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
)

add_library(GTESTMAIN_STATIC STATIC IMPORTED)
set_target_properties(GTESTMAIN_STATIC PROPERTIES
	IMPORTED_LOCATION "${GTEST_MAIN_LIBRARIES}"
	INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
)

find_package(Threads QUIET)
if(TARGET Threads::Threads)
    set_target_properties(GTESTCORE_STATIC PROPERTIES
        INTERFACE_LINK_LIBRARIES Threads::Threads)
endif()

add_dependencies(GTESTCORE_STATIC GTest-EP)
add_dependencies(GTESTMAIN_STATIC GTest-EP)

