
get_cmake_property(variableNames VARIABLES)
set(VARLIST "")
foreach (variableName ${variableNames})
	get_property(ISADV CACHE ${variableName} PROPERTY ADVANCED SET)
	get_property(TYPE CACHE ${variableName} PROPERTY TYPE)
	set(VARLIST "${VARLIST}\n\tprint(${ISADV}, \"${TYPE}\", \"${variableName}\", R\"VAR(${${variableName}})VAR\");")
endforeach()

set(VARLIST "${VARLIST}" PARENT_SCOPE)

#configure_file( ${CMAKE_SOURCE_DIR}/src/carl/util/CMakeOptions.cpp.in ${CMAKE_SOURCE_DIR}/src/carl/util/CMakeOptions.cpp )
