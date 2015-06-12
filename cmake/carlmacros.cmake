# File: macros.cmake
# Authors: Igor N. Bongartz
# Erstellt: 2015-06-11
# Version: 2015-06-11
#
# This file contains several macros which are used in this project. Notice that several are copied straight from web ressources.

# ===================================
# List handling macros http://www.cmake.org/pipermail/cmake/2004-June/005187.html 12.6.2015
# ===================================

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

#MACRO from Stackoverflow (http://stackoverflow.com/questions/7787823/cmake-how-to-get-the-name-of-all-subdirectories-of-a-directory) 10.6.15
MACRO(ListSubDirs result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

#FUNCTION collect_files FOR RECURSION - input "no_subdir"
function(collect_files prefix dir_name dir_path subdir)
	#determine if the are in the dir or subdir
	if(NOT ${subdir} STREQUAL no_subdir)
		set(path ${dir_path}/${subdir})
		set(name ${dir_name}_${subdir})
	else()
		set(path ${dir_path})
		set(name ${dir_name})
	endif()

	#Recursion for subdirectories
  ListSubDirs(subdir ${CMAKE_CURRENT_SOURCE_DIR}/${path})

	foreach(subname ${subdir})

		collect_files(${prefix} ${name} ${path} ${subname})

		#Update upper prefix_name_...
		list(APPEND ${prefix}_${name}_headers ${${prefix}_${name}_${subname}_headers})
		list(APPEND ${prefix}_${name}_sources ${${prefix}_${name}_${subname}_sources})
	endforeach()

	#Headers
	file(GLOB headers  LIST_DIRECTORIES true ${path}/*.h ${path}/*.tpp)
	foreach(header ${headers})
		get_filename_component(reduced_header ${header} NAME)
		list(APPEND ${prefix}_${name}_headers ${path}/${reduced_header})
	endforeach()

	#Sources
	file(GLOB sources ${path}/*.cpp LIST_DIRECTORIES true)
	foreach(source ${sources})
		get_filename_component(reduced_source ${source} NAME)
		list(APPEND ${prefix}_${name}_sources ${path}/${reduced_source})
	endforeach()

	#Configure only if it exists
	if(EXISTS ${CMAKE_SOURCE_DIR}/src/${prefix}/${path}/config.h.in)
		configure_file(${CMAKE_SOURCE_DIR}/src/${prefix}/${path}/config.h.in ${CMAKE_SOURCE_DIR}/src/${prefix}/${path}/config.h)
	endif()

  #ALTERNATE VERSION
  if(FALSE)
      # Alternate version, take all documents even recusiv and do everything in one loop
      file(GLOB_RECURSE subfiles RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${path} ${path}/*)

      foreach(subfile ${subfiles})
        #message(STATUS "think about ${subfile}")
        if(${subfile} MATCHES ".*([.]in)")
          #message(STATUS "noticed config ${subfile}")
          get_filename_component(subfile_name ${subfile} NAME_WE)
          configure_file(${CMAKE_SOURCE_DIR}/src/${prefix}/${path}/${subfile} ${CMAKE_SOURCE_DIR}/src/${prefix}/${path}/${subfile_name}.h)

        elseif((${subfile} MATCHES ".*([.]h)") OR (${subfile} MATCHES ".*([.]tpp)"))
          get_filename_component(subdir ${subfile} DIRECTORY)
          if(NOT ${subdir} STREQUAL "")
            #message(STATUS "noticed dir for headerfiles ${subdir}")
            LIST_APPEND_UNIQUE(${prefix}_${name}_subdir ${subdir})
            list(APPEND ${prefix}_${name}_${subdir}_headers ${subfile})
          endif()
            list(APPEND ${prefix}_${name}_headers ${subfile})

        elseif(${subfile} MATCHES ".*([.]cpp)")
          message(STATUS "noticed source ${subfile}")
          list(APPEND ${prefix}_${name}_sources ${subfile})
        endif()
      endforeach()

      message(STATUS "display headers ${${prefix}_${name}_headers}")
      message(STATUS "display src ${${prefix}_${name}_sources}")



      foreach(subdir ${${prefix}_${name}_subdir})
        install(FILES			${${prefix}_${name}_${subdir}_headers}
          DESTINATION		include/${prefix}/${subdir}/${path})
      endforeach()
  endif()

	#Install
	install(FILES			${${prefix}_${name}_headers}
			DESTINATION		include/${prefix}/${path})

	#SET the scope
	set(${prefix}_${name}_headers ${${prefix}_${name}_headers} PARENT_SCOPE)
	set(${prefix}_${name}_sources ${${prefix}_${name}_sources} PARENT_SCOPE)
endfunction(collect_files)
