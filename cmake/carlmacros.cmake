# File: macros.cmake
# Authors: Igor N. Bongartz
# Erstellt: 2015-06-11
# Version: 2015-06-11
#
# This file contains several macros which are used in this project. Notice that several are copied straight from web ressources.

# Macro
# List handling macros
# Source: http://www.cmake.org/pipermail/cmake/2004-June/005187.html 12.6.2015

MACRO(LIST_PREPEND var value)
     SET(${var} ${value} ${${var}})
ENDMACRO(LIST_PREPEND)

MACRO(LIST_PREPEND_UNIQUE var value)
     SET(LIST_ADD_UNIQUE_FLAG 0)
     FOREACH(i ${${var}})
         IF ("${i}" MATCHES "${value}")
             SET(LIST_ADD_UNIQUE_FLAG 1)
         ENDIF("${i}" MATCHES "${value}")
     ENDFOREACH(i)
     IF(NOT LIST_ADD_UNIQUE_FLAG)
         SET(${var} ${value} ${${var}})
     ENDIF(NOT LIST_ADD_UNIQUE_FLAG)
ENDMACRO(LIST_PREPEND_UNIQUE)

MACRO(LIST_APPEND var value)
     SET(${var} ${${var}} ${value})
ENDMACRO(LIST_APPEND)

MACRO(LIST_APPEND_UNIQUE var value)
     SET(LIST_ADD_UNIQUE_FLAG 0)
     FOREACH(i ${${var}})
         IF ("${i}" MATCHES "${value}")
             SET(LIST_ADD_UNIQUE_FLAG 1)
         ENDIF("${i}" MATCHES "${value}")
     ENDFOREACH(i)
     IF(NOT LIST_ADD_UNIQUE_FLAG)
         SET(${var} ${${var}} ${value})
     ENDIF(NOT LIST_ADD_UNIQUE_FLAG)
ENDMACRO(LIST_APPEND_UNIQUE)

# Function
# collect_files in subdirectories and save them into variables
# Igor Bongartz 06.2015

function(collect_files prefix name)
  file(GLOB_RECURSE subfiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${name} ${name}/*)

  foreach(subfile ${subfiles})
    if(${subfile} MATCHES ".*([.]in)")
      string(REGEX REPLACE ".in" "" subfile_name ${subfile})
      configure_file(${CMAKE_SOURCE_DIR}/src/${prefix}/${name}/${subfile} ${CMAKE_SOURCE_DIR}/src/${prefix}/${name}/${subfile_name})

    elseif((${subfile} MATCHES ".*([.]h)") OR (${subfile} MATCHES ".*([.]tpp)"))
      get_filename_component(subdir ${subfile} DIRECTORY)
      if(NOT ${subdir} STREQUAL "")
        LIST_APPEND_UNIQUE(${prefix}_${name}_subdir ${subdir})
        list(APPEND ${prefix}_${name}_${subdir}_headers ${subfile})
      endif()
        list(APPEND ${prefix}_${name}_headers ${subfile})

    elseif(${subfile} MATCHES ".*([.]cpp)")
      list(APPEND ${prefix}_${name}_sources ${name}/${subfile})
    endif()
  endforeach()

  foreach(subdir ${${prefix}_${name}_subdir})
    install(FILES			${${prefix}_${name}_${subdir}_headers}
      DESTINATION		include/${prefix}/${subdir})
  endforeach()

	#Install
	install(FILES			${${prefix}_${name}_headers}
			DESTINATION		include/${prefix}/${name})

	#SET the scope
	set(${prefix}_${name}_headers ${${prefix}_${name}_headers} PARENT_SCOPE)
	set(${prefix}_${name}_sources ${${prefix}_${name}_sources} PARENT_SCOPE)
endfunction(collect_files)
