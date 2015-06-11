# File: macros.cmake
# Authors: Igor N. Bongartz
# Erstellt: 2015-06-11
# Version: 2015-06-11
#
# This file contains several macros which are used in this project. Notice that several are copied straight from web ressources.


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
function(collect_files dir_name dir_path subdir)
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

		collect_files(${name} ${path} ${subname})

		#Update upper carl_lib
		list(APPEND carl_lib_${name}_headers ${carl_lib_${name}_${subname}_headers})
		list(APPEND carl_lib_${name}_sources ${carl_lib_${name}_${subname}_sources})
	endforeach()

	#Headers
	file(GLOB headers  LIST_DIRECTORIES true ${path}/*.h ${path}/*.tpp)
	foreach(header ${headers})
		get_filename_component(reduced_header ${header} NAME)
		list(APPEND carl_lib_${name}_headers ${path}/${reduced_header})
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
endfunction(collect_files)
