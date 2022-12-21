if(NOT FORCE_SHIPPED_RESOURCES)
	find_path(GTEST_INCLUDE_DIR NAMES gtest/gtest.h)
	find_library(GTEST_LIB NAMES gtest)
	find_library(GTEST_MAIN_LIBRARIES NAMES gtest_main)
endif()

set(GTest_FOUND_SYSTEM FALSE)
if(GTEST_INCLUDE_DIR AND GTEST_LIB AND GTEST_MAIN_LIBRARIES)
    set(GTest_FOUND_SYSTEM TRUE)
endif()

if(NOT GTest_FOUND_SYSTEM)
	ExternalProject_Add(
    GTest-EP
	URL https://github.com/google/googletest/archive/release-${GTEST_VERSION}.zip
	URL_HASH SHA1=${GTEST_ZIPHASH}
	CMAKE_ARGS
          -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
		  -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
	BUILD_COMMAND cmake --build . --config ${CMAKE_BUILD_TYPE} --target gtest
	COMMAND cmake --build . --config ${CMAKE_BUILD_TYPE} --target gtest_main
	BUILD_BYPRODUCTS 
		<INSTALL_DIR>/lib/libgtest.a 
		<INSTALL_DIR>/lib/libgtest_main.a
	)

	set(GTEST_INCLUDE_DIR "${CMAKE_BINARY_DIR}/resources/include")
	set(GTEST_LIB "${CMAKE_BINARY_DIR}/resources/lib/libgtest.a")
	set(GTEST_MAIN_LIBRARIES "${CMAKE_BINARY_DIR}/resources/lib/libgtest_main.a")

endif() 

add_library(GTESTCORE_STATIC STATIC IMPORTED GLOBAL)
set_target_properties(GTESTCORE_STATIC PROPERTIES
	IMPORTED_LOCATION "${GTEST_LIB}"
	INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
)

add_library(GTESTMAIN_STATIC STATIC IMPORTED GLOBAL)
set_target_properties(GTESTMAIN_STATIC PROPERTIES
	IMPORTED_LOCATION "${GTEST_MAIN_LIBRARIES}"
	INTERFACE_INCLUDE_DIRECTORIES "${GTEST_INCLUDE_DIR}"
)

find_package(Threads QUIET)
if(TARGET Threads::Threads)
    set_target_properties(GTESTCORE_STATIC PROPERTIES
        INTERFACE_LINK_LIBRARIES Threads::Threads)
endif()

if(NOT GTest_FOUND_SYSTEM)
	add_dependencies(GTESTCORE_STATIC GTest-EP)
	add_dependencies(GTESTMAIN_STATIC GTest-EP)
endif()

set(GTEST_FOUND TRUE)

mark_as_advanced(GTEST_INCLUDE_DIR GTEST_LIB GTEST_MAIN_LIBRARIES)