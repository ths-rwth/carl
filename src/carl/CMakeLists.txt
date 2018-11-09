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
configure_everything()

file(GLOB_RECURSE carl_lib_sources "*.cpp")
add_new_libraries(carl ${LIB_VERSION} "${carl_lib_sources}" "resources")

target_include_dirs_from(carl-objects SYSTEM PUBLIC EIGEN3 GMPXX_SHARED GMP_SHARED Boost_SHARED ${CMAKE_DL_LIBS} pthread)
target_link_libraries(carl-shared PUBLIC EIGEN3 GMPXX_SHARED GMP_SHARED Boost_SHARED ${CMAKE_DL_LIBS} pthread)
target_link_libraries(carl-static PUBLIC EIGEN3 GMPXX_STATIC GMP_STATIC Boost_STATIC ${CMAKE_DL_LIBS} pthread)
if(USE_BLISS)
	target_include_dirs_from(carl-objects SYSTEM PUBLIC BLISS_SHARED)
	target_link_libraries(carl-shared PUBLIC BLISS_SHARED)
	target_link_libraries(carl-static PUBLIC BLISS_STATIC)
endif()
if(USE_CLN_NUMBERS)
	target_include_dirs_from(carl-objects SYSTEM PUBLIC CLN_SHARED)
	target_link_libraries(carl-shared PUBLIC CLN_SHARED)
	target_link_libraries(carl-static PUBLIC CLN_STATIC)
endif()
if(USE_COCOA)
	target_include_dirs_from(carl-objects SYSTEM PUBLIC COCOA_SHARED Boost_system_SHARED)
	target_link_libraries(carl-shared PUBLIC COCOA_SHARED Boost_system_SHARED)
	target_link_libraries(carl-static PUBLIC COCOA_STATIC Boost_system_STATIC)
endif()
if(USE_GINAC)
	target_include_dirs_from(carl-objects SYSTEM PUBLIC GINAC_SHARED)
	target_link_libraries(carl-shared PUBLIC GINAC_SHARED)
	target_link_libraries(carl-static PUBLIC GINAC_STATIC)
endif()
if(USE_MPFR_FLOAT)
	target_include_dirs_from(carl-objects SYSTEM PUBLIC MPFR_SHARED)
	target_link_libraries(carl-shared PUBLIC MPFR_SHARED)
	target_link_libraries(carl-static PUBLIC MPFR_STATIC)
endif()
<<<<<<< HEAD
if(USE_Z3_NUMBERS OR USE_Z3_RANS OR COMPARE_WITH_Z3)
	target_link_libraries(carl-shared PUBLIC Z3_SHARED)
	target_link_libraries(carl-static PUBLIC Z3_STATIC)
endif()

#set_target_properties( lib_carl_static PROPERTIES LINK_SEARCH_END_STATIC TRUE )
#set_target_properties( lib_carl_static PROPERTIES LINK_SEARCH_START_STATIC TRUE )
=======
if(USE_Z3_NUMBERS OR USE_Z3_RANS OR COMPARE_WITH_Z3)
	target_link_libraries(lib_carl PUBLIC Z3_SHARED)
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
	if(USE_BLISS)
		target_link_libraries(lib_carl_static PUBLIC BLISS_STATIC)
	endif()
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
	if(USE_Z3_NUMBERS OR USE_Z3_RANS OR COMPARE_WITH_Z3)
		target_link_libraries(lib_carl_static PUBLIC Z3_STATIC)
	endif()

	set(CARL_TARGETS lib_carl lib_carl_static CACHE STRING INTERNAL FORCE)
else()
	set(CARL_TARGETS lib_carl CACHE STRING INTERNAL FORCE)
endif()
>>>>>>> some attempts to fix build...

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
