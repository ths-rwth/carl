# include directories.
include_directories( ${LOG4CPLUS_INCLUDE_DIR}

)

# include core in library. 
add_subdirectory(core)

# Compose all the sources for the aritmetic library.
set(arithmetic_lib_sources 
	${arithmetic_lib_core_sources}
)

# set linking flags
set(STATICLIB "SHARED")
if(${STATICLIB_SWITCH} STREQUAL "ON")
	set(STATICLIB "STATIC")
endif()

add_library(lib_arithmetic ${STATICLIB} ${arithmetic_lib_sources} )
set_target_properties( lib_arithmetic PROPERTIES
		# create *nix style library versions + symbolic links
		VERSION "${PROJECT_VERSION}"
		SOVERSION "${PROJECT_VERSION}"
		# allow creating static and shared libs without conflicts
		# CLEAN_DIRECT_OUTPUT 1
		# avoid conflicts between library and binary target names
		OUTPUT_NAME arithmetic
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
if(${STATICLIB_SWITCH} STREQUAL "ON")
  set_target_properties( lib_arithmetic PROPERTIES LINK_SEARCH_END_STATIC TRUE )
  set_target_properties( lib_arithmetic PROPERTIES LINK_SEARCH_START_STATIC TRUE )
endif()
target_link_libraries( lib_arithmetic ${libraries} ) # libraries definied in top-level CMakeLists.txt


# Install destination and permissions for the library
# # # # # # # # # # # # # # # # # # # # # # # # # # #

install( TARGETS lib_arithmetic DESTINATION lib PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
)

