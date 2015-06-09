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



# simplify the CMakeLists.txt src/carl/*
foreach(name converter core interval formula)

	#Subdir Headers - Sources -Install
	foreach(subname bitvector)

		if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/src/carl/${name}/${subname})
			#Header Subdir
			file(GLOB subheaders  LIST_DIRECTORIES true ${name}/${subname}/*.h ${name}/${subname}/*.tpp)
			foreach(subheader ${subheaders})
				get_filename_component(reduced_subheader ${subheader} NAME)
				list(APPEND carl_lib_${name}_${subname}_headers ${reduced_subheader})
			endforeach()

			#Sources Subdir
			file(GLOB subsources ${name}/${subname}/*.cpp LIST_DIRECTORIES true)
			foreach(subsource ${subsources})
				get_filename_component(reduced_subsource ${subsource} NAME)
				list(APPEND carl_lib_${name}_${subname}_sources ${name}/${subname}/${reduced_subsource})
			endforeach()

			#Install Subdir
			install(FILES			${carl_lib_${name}_${subname}_headers}
					DESTINATION		include/carl/${name}/${subname})

			#Update upper carl_lib
			list(APPEND carl_lib_${name}_headers ${carl_lib_${name}_${subname}_headers})
			list(APPEND carl_lib_${name}_sources ${carl_lib_${name}_${subname}_sources})

			#HELP
			#message(STATUS ${name} " display subheaders: " ${carl_lib_${name}_${subname}_headers})
			#message(STATUS ${name} " display subsources: " ${carl_lib_${name}_${subname}_sources})
		endif()
	endforeach()

	#Headers
	file(GLOB headers  LIST_DIRECTORIES true ${name}/*.h ${name}/*.tpp ${name}/*/*.h ${name}/*/*.tpp)
	foreach(header ${headers})
		get_filename_component(reduced_header ${header} NAME)
		list(APPEND carl_lib_${name}_headers ${reduced_header})
	endforeach()


	#Sources
	file(GLOB sources ${name}/*.cpp LIST_DIRECTORIES true)
	foreach(source ${sources})
		get_filename_component(reduced_source ${source} NAME)
		list(APPEND carl_lib_${name}_sources ${name}/${reduced_source})
	endforeach()

	#Configure
	configure_file(${CMAKE_SOURCE_DIR}/src/carl/${name}/config.h.in ${CMAKE_SOURCE_DIR}/src/carl/${name}/config.h})

	#Install
	install(FILES			${carl_lib_${name}_headers}
			DESTINATION		include/carl/${name})

	#HELP
	#message(STATUS ${name} " display headers: " ${carl_lib_${name}_headers})
	#message(STATUS ${name} " display sources: " ${carl_lib_${name}_sources})
endforeach()


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
