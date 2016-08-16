
get_cmake_property(variableNames VARIABLES)
set(VARLIST "")
foreach (variableName ${variableNames})
	get_property(ISADV CACHE ${variableName} PROPERTY ADVANCED SET)
	get_property(TYPE CACHE ${variableName} PROPERTY TYPE)
	if((NOT ${ISADV}) AND (NOT ${TYPE} MATCHES "INTERNAL") AND (NOT ${TYPE} MATCHES "STATIC"))
		set(VARLIST "${VARLIST}\n\t p(\"${variableName}\", R\"VAR(${${variableName}})VAR\");")
	endif()
endforeach()

configure_file( ${CMAKE_SOURCE_DIR}/src/carl/util/CMakeOptions.cpp.in ${CMAKE_SOURCE_DIR}/src/carl/util/CMakeOptions.cpp )
