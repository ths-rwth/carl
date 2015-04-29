# include directories.
include_directories( 
	${include_dirs}
)

# include core in library. 
add_subdirectory(core)
add_subdirectory(formula)
add_subdirectory(util)
add_subdirectory(converter)
add_subdirectory(numbers)


#include groebner and cad modules in library.
add_subdirectory(groebner)
add_subdirectory(cad)
add_subdirectory(interval)



# Compose all the sources for the aritmetic library.
set(carl_lib_sources 
    ${carl_lib_cad_sources}
	${carl_lib_converter_sources}
	${carl_lib_core_sources}
	${carl_lib_formula_sources}
	${carl_lib_number_sources}
	${carl_lib_util_sources}
	${carl_lib_number_sources}
	${carl_lib_interval_sources}
	${carl_lib_groebner_sources}
)

add_library(lib_carl SHARED ${carl_lib_sources})
set_target_properties( lib_carl PROPERTIES
	# create *nix style library versions + symbolic links
	VERSION "${PROJECT_VERSION}"
	SOVERSION "${PROJECT_VERSION}"
	# allow creating static and shared libs without conflicts
	CLEAN_DIRECT_OUTPUT 1
	# avoid conflicts between library and binary target names
	OUTPUT_NAME carl
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	# we currently do not use public header option, as it doesnt support multi-directory headers
)
target_link_libraries( lib_carl ${libs_dynamic} )

if(BUILD_STATIC)
	add_library(lib_carl_static STATIC ${carl_lib_sources})
	set_target_properties( lib_carl_static PROPERTIES
		# create *nix style library versions + symbolic links
		VERSION "${PROJECT_VERSION}"
		SOVERSION "${PROJECT_VERSION}"
		# allow creating static and shared libs without conflicts
		CLEAN_DIRECT_OUTPUT 1
		# avoid conflicts between library and binary target names
		OUTPUT_NAME carl
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		# we currently do not use public header option, as it doesnt support multi-directory headers
	)
	target_link_libraries( lib_carl_static ${libs_static} )
	set(CARL_TARGETS lib_carl lib_carl_static CACHE STRING INTERNAL FORCE)
else()
	set(CARL_TARGETS lib_carl CACHE STRING INTERNAL FORCE)
endif()

cotire(carl)

# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/carl/config.h.in 
    ${CMAKE_SOURCE_DIR}/src/carl/config.h
)  

# Install destination and permissions for the library
# # # # # # # # # # # # # # # # # # # # # # # # # # #

install( TARGETS ${CARL_TARGETS}
	EXPORT carlTargets
	LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    COMPONENT shlib
	PERMISSIONS
		OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE
)


