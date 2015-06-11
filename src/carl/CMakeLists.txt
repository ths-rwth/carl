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


# # # # # # # # # # # # # # # # # # # # # #
# Simplify CMakeLists.txt in the subdirectories
# HELP
# SIMPLIFY
# # # # # # # # # # # # # # # # # # # # # #

# # # # # # # # # # # # # # # # # # # # # #
# HELP
# # # # # # # # # # # # # # # # # # # # # #

#MACRO from Stackoverflow (http://stackoverflow.com/questions/7787823/cmake-how-to-get-the-name-of-all-subdirectories-of-a-directory) 10.6.15
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

#FUNCTION compute_install FOR RECURSION - input "no_subdir
function(compute_install dir subdir)
	#determine if the are in the dir or subdir
	if(NOT ${subdir} STREQUAL no_subdir)
		set(path ${${dir}}/${${subdir}})
		set(name ${${dir}}_${${subdir}})
	else()
		set(path ${${dir}})
		set(name ${${dir}})
	endif()

	#Recursion for subdirectories
	SUBDIRLIST(subdir ${CMAKE_CURRENT_SOURCE_DIR}/${path})

	#Subdir Headers - Sources -Install
	foreach(subname ${subdir})
		message(STATUS "next we compute the help-function for " ${name} " with focus on " ${subname})

		compute_install(name subname)

		#Update upper carl_lib
		list(APPEND carl_lib_${name}_headers ${carl_lib_${name}_${subname}_headers})
		list(APPEND carl_lib_${name}_sources ${carl_lib_${name}_${subname}_sources})
	endforeach()

	#Headers
	file(GLOB headers  LIST_DIRECTORIES true ${path}/*.h ${path}/*.tpp)
	foreach(header ${headers})
		get_filename_component(reduced_header ${header} NAME)
		list(APPEND carl_lib_${name}_headers ${reduced_header})
	endforeach()

	#Sources
	file(GLOB sources ${path}/*.cpp LIST_DIRECTORIES true)
	foreach(source ${sources})
		get_filename_component(reduced_source ${source} NAME)
		list(APPEND carl_lib_${name}_sources ${path}/${reduced_source})
	endforeach()

	#Configure only if it exists
	if(EXISTS ${CMAKE_SOURCE_DIR}/src/carl/${path}/config.h.in)
		configure_file(${CMAKE_SOURCE_DIR}/src/carl/${path}/config.h.in ${CMAKE_SOURCE_DIR}/src/carl/${path}/config.h)
	endif()

	#Install
	install(FILES			${carl_lib_${name}_headers}
			DESTINATION		include/carl/${path})

	#SET the scope of carl_lib_...
	set(carl_lib_${name}_headers ${carl_lib_${name}_headers} PARENT_SCOPE)
	set(carl_lib_${name}_sources ${carl_lib_${name}_sources} PARENT_SCOPE)
endfunction(compute_install)

# # # # # # # # # # # # # # # # # # # # # #
# SIMPLIFY
# # # # # # # # # # # # # # # # # # # # # #

foreach(name cad converter core formula groebner interval numbers util)
	compute_install(name no_subdir)
endforeach()

# # # # # # # # # # # # # # # # # # # # # #
# END SIMPLIFY
# # # # # # # # # # # # # # # # # # # # # #

# Compose all the sources for the aritmetic library.
set(carl_lib_sources
    ${carl_lib_cad_sources}
	${carl_lib_converter_sources}
	${carl_lib_core_sources}
	${carl_lib_formula_sources}
	${carl_lib_numbers_sources}
	${carl_lib_util_sources}
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
