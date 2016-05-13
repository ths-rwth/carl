# include directories.
message(STATUS "Including ${include_dirs}")
include_directories(
	${include_dirs}
)

# include util in library.
add_subdirectory(util)

# Simplified access to *.h and *.cpp
foreach(name cad converter core formula groebner interval numbers util)
	collect_files(carl ${name})
endforeach()


# Compose all the sources for the aritmetic library.
set(carl_lib_sources
    ${carl_cad_sources}
	${carl_converter_sources}
	${carl_core_sources}
	${carl_formula_sources}
	${carl_numbers_sources}
	${carl_util_sources}
	${carl_interval_sources}
	${carl_groebner_sources}
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
	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/src"
	# we currently do not use public header option, as it doesnt support multi-directory headers
)
target_link_libraries(lib_carl PUBLIC CLN_SHARED GMP_SHARED GMPXX_SHARED dl GINAC_SHARED)

if(BUILD_STATIC)
	add_library(lib_carl_static STATIC ${carl_lib_sources})
	add_dependencies(lib_carl_static resources)
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
		INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/src"
		# we currently do not use public header option, as it doesnt support multi-directory headers
	)

	# Options for static linking
	message("-- linking static")
	if (APPLE)
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mmacosx-version-min=10.7")
	else()
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
	endif()

	#target_link_libraries( lib_carl_static ${carl_LIBRARIES_STATIC} )
	target_link_libraries(lib_carl_static CLN_STATIC)
	target_link_libraries(lib_carl_static GMP_STATIC GMPXX_STATIC)
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

if (CLANG_TIDY)
	add_custom_target(tidy
		COMMAND clang-tidy -p ../../build/compile_commands.json ${CLANG_TIDY_CHECKS} -header-filter='.*' ${carl_lib_sources}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/carl/
	)
endif()
