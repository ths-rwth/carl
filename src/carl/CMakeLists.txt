# include directories.
#message(STATUS "Including ${include_dirs}")
#include_directories(
#	${include_dirs}
#)

set(LIB_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

# include util in library.
add_subdirectory(util)

include(GNUInstallDirs)
# Configure all .in files
file(GLOB_RECURSE carl_lib_tpl "*.in")
foreach(tpl ${carl_lib_tpl})
	string(REGEX REPLACE ".in$" "" file ${tpl})
	configure_file("${tpl}" "${file}")
endforeach()

file(GLOB_RECURSE carl_lib_sources "*.cpp")

add_library(lib_carl SHARED ${carl_lib_sources})
add_dependencies(lib_carl resources)
set_target_properties( lib_carl PROPERTIES
	VERSION "${LIB_VERSION}"
	SOVERSION "${LIB_VERSION}"
	CLEAN_DIRECT_OUTPUT 1
	OUTPUT_NAME carl
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
#	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/src"
)
target_include_directories( lib_carl PUBLIC
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
)
target_link_libraries(lib_carl PUBLIC EIGEN3 GMPXX_SHARED GMP_SHARED Boost_SHARED ${CMAKE_DL_LIBS} pthread)
if(USE_CLN_NUMBERS)
	target_link_libraries(lib_carl PUBLIC CLN_SHARED)
endif()
if(USE_COCOA)
	target_link_libraries(lib_carl PUBLIC COCOA_STATIC Boost_system_SHARED)
endif()
if(USE_GINAC)
	target_link_libraries(lib_carl PUBLIC GINAC_SHARED)
endif()
if(USE_MPFR_FLOAT)
	target_link_libraries(lib_carl PUBLIC MPFR_SHARED)
endif()

if(BUILD_STATIC)
	add_library(lib_carl_static STATIC ${carl_lib_sources})
	add_dependencies(lib_carl_static resources)
	set_target_properties( lib_carl_static PROPERTIES
		VERSION "${LIB_VERSION}"
		SOVERSION "${LIB_VERSION}"
		CLEAN_DIRECT_OUTPUT 1
		OUTPUT_NAME carl
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/src"
	)
        set_target_properties( lib_carl_static PROPERTIES LINK_SEARCH_END_STATIC TRUE )
        set_target_properties( lib_carl_static PROPERTIES LINK_SEARCH_START_STATIC TRUE )

	target_link_libraries(lib_carl_static PUBLIC EIGEN3 GMPXX_STATIC GMP_STATIC Boost_STATIC ${CMAKE_DL_LIBS} pthread)
	if(USE_CLN_NUMBERS)
		target_link_libraries(lib_carl_static PUBLIC CLN_STATIC)
	endif()
	if(USE_COCOA)
		target_link_libraries(lib_carl_static PUBLIC COCOA_STATIC Boost_system_STATIC)
	endif()
	if(USE_GINAC)
		target_link_libraries(lib_carl_static PUBLIC GINAC_STATIC)
	endif()
	if(USE_MPFR_FLOAT)
		target_link_libraries(lib_carl_static PUBLIC MPFR_STATIC)
	endif()

	set(CARL_TARGETS lib_carl lib_carl_static CACHE STRING INTERNAL FORCE)
else()
	set(CARL_TARGETS lib_carl CACHE STRING INTERNAL FORCE)
endif()

cotire(carl)
include(install)

if (CLANG_TIDY)
	add_custom_target(tidy
		COMMAND ${CLANG_TIDY} -p ../../build/compile_commands.json ${CLANG_TIDY_CHECKS} -header-filter='.*' ${carl_lib_sources}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/carl/
	)
	file(GLOB dirs LIST_DIRECTORIES true RELATIVE "${CMAKE_SOURCE_DIR}/src/carl/" "*")
	foreach(dir ${dirs})
		if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/src/carl/${dir}")
			file(GLOB_RECURSE sources_${dir} "${CMAKE_SOURCE_DIR}/src/carl/${dir}/*.cpp")
			add_custom_target(tidy-${dir}
				COMMAND ${CLANG_TIDY} -p ../../build/compile_commands.json ${CLANG_TIDY_CHECKS} -header-filter='.*' ${sources_${dir}}
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/carl/
			)
		endif()
	endforeach()
endif()
