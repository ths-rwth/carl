# include directories.
message(${include_dirs})
include_directories( 
	${include_dirs}
)

# include core in library. 
add_subdirectory(core)
add_subdirectory(util)
add_subdirectory(numbers)


#include groebner and cad modules in library.
add_subdirectory(groebner)
add_subdirectory(cad)
add_subdirectory(interval)



# Compose all the sources for the aritmetic library.
set(carl_lib_sources 
        ${carl_lib_cad_sources}
	${carl_lib_core_sources}
	${carl_lib_util_sources}
	${carl_lib_number_sources}
	${carl_lib_interval_sources}
)


# set linking flags
set(STATICLIB "SHARED")
if(${STATICLIB_SWITCH} STREQUAL "ON")
	set(STATICLIB "STATIC")
endif()

add_library(lib_carl ${STATICLIB} ${carl_lib_sources} )
set_target_properties( lib_carl PROPERTIES
		# create *nix style library versions + symbolic links
		VERSION "${PROJECT_VERSION}"
		SOVERSION "${PROJECT_VERSION}"
		# allow creating static and shared libs without conflicts
		# CLEAN_DIRECT_OUTPUT 1
		# avoid conflicts between library and binary target names
		OUTPUT_NAME carl
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		# we currently do not use public header option, as it doesnt support multi-directory headers
)


if(${STATICLIB_SWITCH} STREQUAL "ON")
  set_target_properties( lib_carl PROPERTIES LINK_SEARCH_END_STATIC TRUE )
  set_target_properties( lib_carl PROPERTIES LINK_SEARCH_START_STATIC TRUE )
endif()

target_link_libraries( lib_carl ${libraries} ) # libraries definied in top-level CMakeLists.txt


# Install destination and permissions for the library
# # # # # # # # # # # # # # # # # # # # # # # # # # #
 

install( TARGETS lib_carl 
	EXPORT carlTargets
	LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    COMPONENT shlib
	PERMISSIONS
		OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE
)


