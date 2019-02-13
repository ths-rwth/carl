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
target_link_libraries(carl-shared PUBLIC EIGEN3 GMPXX_SHARED GMP_SHARED Boost_SHARED ${CMAKE_DL_LIBS} stdc++fs pthread)
target_link_libraries(carl-static PUBLIC EIGEN3 GMPXX_STATIC GMP_STATIC Boost_STATIC ${CMAKE_DL_LIBS} stdc++fs pthread)
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
if(USE_Z3_NUMBERS OR RAN_USE_Z3 OR COMPARE_WITH_Z3)
	target_include_dirs_from(carl-objects SYSTEM PUBLIC Z3_SHARED)
	target_link_libraries(carl-shared PUBLIC Z3_SHARED)
	target_link_libraries(carl-static PUBLIC Z3_STATIC)
endif()

#set_target_properties( lib_carl_static PROPERTIES LINK_SEARCH_END_STATIC TRUE )
#set_target_properties( lib_carl_static PROPERTIES LINK_SEARCH_START_STATIC TRUE )

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
