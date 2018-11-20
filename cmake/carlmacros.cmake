# File: macros.cmake
# Authors: Igor N. Bongartz
# Erstellt: 2015-06-11
# Version: 2015-06-11
#
# This file contains several macros which are used in this project. Notice that several are copied straight from web ressources.

function(set_version major minor)
	execute_process(
		COMMAND git describe --match "[0-9]*.[0-9]*"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_VERSION
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	set(patch "")
	if (GIT_VERSION MATCHES "([0-9]+)\.([0-9]+)(-?.*)")
		set(major ${CMAKE_MATCH_1})
		set(minor ${CMAKE_MATCH_2})
		if (CMAKE_MATCH_3 MATCHES "-([0-9]+)-(g[0-9a-f]+)")
			set(patch_full "${CMAKE_MATCH_1}-${CMAKE_MATCH_2}")
			set(patch "${CMAKE_MATCH_1}")
		endif()
	else()
		message(STATUS "Could not parse version from git, using default ${major}.${minor}")
	endif()
	
	set(PROJECT_VERSION_MAJOR ${major} PARENT_SCOPE)
	set(PROJECT_VERSION_MINOR ${minor} PARENT_SCOPE)
	set(PROJECT_VERSION_PATCH ${patch} PARENT_SCOPE)
	if(patch)
		set(PROJECT_VERSION_FULL "${major}.${minor}.${patch_full}" PARENT_SCOPE)
		set(PROJECT_VERSION "${major}.${minor}.${patch}" PARENT_SCOPE)
	else()
		set(PROJECT_VERSION_FULL "${major}.${minor}" PARENT_SCOPE)
		set(PROJECT_VERSION "${major}.${minor}" PARENT_SCOPE)
	endif()
endfunction(set_version)

function(add_imported_library_interface name include)
	add_library(${name} INTERFACE IMPORTED)
	set_target_properties(${name} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${include}")
endfunction(add_imported_library_interface)

function(add_imported_library name type lib include)
	# Workaround from https://cmake.org/Bug/view.php?id=15052
	file(MAKE_DIRECTORY "${include}")
	if("${lib}" STREQUAL "")
		if("${type}" STREQUAL "SHARED")
			add_library(${name} INTERFACE IMPORTED)
			set_target_properties(${name} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${include}")
		endif()
	else()
		set(lib_list "${lib}")
		list(LENGTH lib_list NumFiles)
		if(NumFiles GREATER 1)
			add_library(${name}_${type} INTERFACE IMPORTED GLOBAL)
			set(shortnames "${ARGN}")
			set(libs "")
			math(EXPR range "${NumFiles}-1")
			foreach(index RANGE ${range})
				list(GET lib_list ${index} l)
				list(GET shortnames ${index} shortname)
				add_imported_library("${name}_${shortname}" ${type} ${l} ${include})
				list(APPEND libs "${name}_${shortname}_${type}")
				# only from cmake 3.3 https://github.com/ceph/ceph/pull/7128
				#add_dependencies(${name}_${type} ${name}_${shortname}_${type})
			endforeach()
			set_target_properties(${name}_${type} PROPERTIES INTERFACE_LINK_LIBRARIES "${libs}")
			set_target_properties(${name}_${type} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${include}")
		else()
			add_library(${name}_${type} ${type} IMPORTED GLOBAL)
			set_target_properties(${name}_${type} PROPERTIES IMPORTED_LOCATION "${lib}")
			set_target_properties(${name}_${type} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${include}")
			if(WIN32)
				string(REPLACE "dll" "lib" IMPLIB ${lib})
				set_target_properties(${name}_${type} PROPERTIES IMPORTED_IMPLIB "${IMPLIB}")
			endif()
		endif()
	endif()
endfunction(add_imported_library)

function(export_option name)
	list(APPEND EXPORTED_OPTIONS "${name}")
	set(EXPORTED_OPTIONS "${EXPORTED_OPTIONS}" PARENT_SCOPE)
endfunction(export_option)

function(_export_target_add_exec output TARGET)
	set(${output} "${${output}}
	add_executable(${TARGET} IMPORTED)" PARENT_SCOPE)
endfunction(_export_target_add_exec)
function(_export_target_add_lib output TARGET TYPE)
	set(${output} "${${output}}
	add_library(${TARGET} ${TYPE} IMPORTED)" PARENT_SCOPE)
endfunction(_export_target_add_lib)
function(_export_target_start output TARGET)
	set(${output} "${${output}}

if(NOT TARGET ${TARGET})" PARENT_SCOPE)
endfunction(_export_target_start)
function(_export_target_end output)
	set(${output} "${${output}}
endif()" PARENT_SCOPE)
endfunction(_export_target_end)

macro(_export_target_set_prop output TARGET REQUIRED PROPERTY)
	get_target_property(VALUE ${TARGET} ${PROPERTY})
	set(extra_args ${ARGN})
	list(GET extra_args 0 NEWPROPERTY)
	if(NOT NEWPROPERTY)
		set(NEWPROPERTY ${PROPERTY})
	endif()
	if(VALUE)
		set(${output} "${${output}}
	set_target_properties(${TARGET} PROPERTIES ${NEWPROPERTY} \"${VALUE}\")")
	elseif(${REQUIRED})
		message(STATUS "Did not find ${PROPERTY} of ${TARGET} for export.")
	endif()
endmacro(_export_target_set_prop)

macro(export_target output target)
	if(TARGET ${target})
		get_target_property(TYPE ${target} TYPE)
		_export_target_start(${output} ${target})
		if(TYPE STREQUAL "EXECUTABLE")
			_export_target_add_exec(${output} ${target})
			_export_target_set_prop(${output} ${target} TRUE IMPORTED_LOCATION)
		elseif(TYPE STREQUAL "SHARED_LIBRARY")
			_export_target_add_lib(${output} ${target} SHARED)
			_export_target_set_prop(${output} ${target} TRUE IMPORTED_LOCATION)
			_export_target_set_prop(${output} ${target} FALSE INTERFACE_INCLUDE_DIRECTORIES)
			_export_target_set_prop(${output} ${target} FALSE LINK_INTERFACE_LIBRARIES)
		elseif(TYPE STREQUAL "STATIC_LIBRARY")
			_export_target_add_lib(${output} ${target} STATIC)
			_export_target_set_prop(${output} ${target} TRUE IMPORTED_LOCATION)
			_export_target_set_prop(${output} ${target} FALSE INTERFACE_INCLUDE_DIRECTORIES)
			_export_target_set_prop(${output} ${target} FALSE LINK_INTERFACE_LIBRARIES IMPORTED_LINK_INTERFACE_LIBRARIES)
		elseif(TYPE STREQUAL "INTERFACE_LIBRARY")
			_export_target_add_lib(${output} ${target} INTERFACE)
			_export_target_set_prop(${output} ${target} TRUE INTERFACE_INCLUDE_DIRECTORIES)
			_export_target_set_prop(${output} ${target} FALSE INTERFACE_LINK_LIBRARIES)
		else()
			message(STATUS "Unknown type ${TYPE}")
		endif()
		if(NOT "${ARGN}" STREQUAL "")
			set(${output} "${${output}}
	set_target_properties(${target} PROPERTIES IMPORTED_LINK_INTERFACE_LIBRARIES \"${ARGN}\")")
		endif()
		_export_target_end(${output})
	endif()
endmacro(export_target)

macro(export_target_recursive output target RECURSE_ON)
	export_target(${output} ${target})
	get_target_property(deps ${target} ${RECURSE_ON})
	foreach(d ${deps})
		export_target(${output} ${d})
	endforeach()
endmacro(export_target_recursive)

macro(load_library group name version)
    string(TOUPPER ${name} LIBNAME)
    set(CMAKE_FIND_LIBRARY_SUFFIXES "${DYNAMIC_EXT};${STATIC_EXT}")
    set(Boost_USE_STATIC_LIBS OFF)
    find_package(${name} ${version} ${ARGN} QUIET)
    if(${name}_FOUND OR ${LIBNAME}_FOUND)
        if (${name}_FOUND)
            set(LIBNAME ${name})
        endif()

		if(LIBNAME MATCHES "Boost")
			add_imported_library(${LIBNAME} SHARED "${${LIBNAME}_LIBRARIES}" "${${LIBNAME}_INCLUDE_DIR}" "${BOOST_COMPONENTS}")
		else()
			add_imported_library(${LIBNAME} SHARED "${${LIBNAME}_LIBRARY}" "${${LIBNAME}_INCLUDE_DIR}")
		endif()

        unset(${LIBNAME}_FOUND CACHE)
        unset(${LIBNAME}_INCLUDE_DIR CACHE)
        unset(${LIBNAME}_LIBRARY CACHE)

        set(CMAKE_FIND_LIBRARY_SUFFIXES "${STATIC_EXT};${DYNAMIC_EXT}")
        set(Boost_USE_STATIC_LIBS ON)
        if (ARGN)
            list(REMOVE_ITEM ARGN "REQUIRED")
        endif()
        find_package(${name} ${version} ${ARGN} QUIET)
        if(${LIBNAME}_FOUND)
			if(LIBNAME MATCHES "Boost")
				add_imported_library(${LIBNAME} STATIC "${${LIBNAME}_LIBRARIES}" "${${LIBNAME}_INCLUDE_DIR}" "${BOOST_COMPONENTS}")
			else()
				add_imported_library(${LIBNAME} STATIC "${${LIBNAME}_LIBRARY}" "${${LIBNAME}_INCLUDE_DIR}")
			endif()
        endif()

        unset(${LIBNAME}_LIBRARY CACHE)
    endif()
endmacro(load_library)

macro(get_include_dir var name)
	get_target_property(INCLUDE_DIR ${name} INTERFACE_INCLUDE_DIRECTORIES)
	set(${var} ${${var}}:${INCLUDE_DIR})
endmacro(get_include_dir)

macro(add_new_libraries name version sources dependencies)
	add_library(${name}-objects OBJECT ${sources})
	add_dependencies(${name}-objects ${dependencies})

	add_library(${name}-shared SHARED $<TARGET_OBJECTS:${name}-objects>)
	set_target_properties(${name}-shared PROPERTIES
		VERSION "${version}"
		SOVERSION "${version}"
		CLEAN_DIRECT_OUTPUT 1
		OUTPUT_NAME ${name}
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	)

	add_library(${name}-static STATIC $<TARGET_OBJECTS:${name}-objects>)
	set_target_properties(${name}-static PROPERTIES
		VERSION "${version}"
		SOVERSION "${version}"
		CLEAN_DIRECT_OUTPUT 1
		OUTPUT_NAME ${name}
		LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
		ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	)

	add_custom_target(${name} DEPENDS ${name}-shared ${name}-static)
endmacro(add_new_libraries)