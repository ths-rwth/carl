
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

set(carl_lib_util_headers 
	BitVector.h
	CompactTree.h
	debug.h
	Heap.h
	SFINAE.h
	tree.h
	Cache.h
	TermAdditionManager.h
	Common.h
	parser/Common.h
	parser/Parser.h
	parser/FormulaParser.h
	parser/PolynomialParser.h
	parser/RationalFunctionParser.h
) 
# Source files in here
set(carl_lib_util_sources 
	util/BitVector.cpp
	util/debug.cpp
	util/CMakeOptions.cpp
	
PARENT_SCOPE)

# Installation of header files
install(FILES			${carl_lib_util_headers}
		DESTINATION		include/carl/util
)
