# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                             #
# File: doxygen-conf/CMakeLists.txt                           #
#                                                             #
# Project name: CARL                                          #
# Contact: Florian Corzilius <corzilius@cs.rwth-aachen.de>,   #
#          Ulrich Loup <loup@cs.rwth-aachen.de>               #
# Project sourceforge page: http://smtrat.sourceforge.net/    #
# Development status: beta                                    #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Version: 2012-02-21                                         #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# Quick self-configuration if only doc is built
if("${PROJECT_NAME}" STREQUAL "")
	cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
	project(CArL-doxygen NONE)
	set(CMAKE_CURRENT_BINARY_DIR "build")
	set(CMAKE_SOURCE_DIR "..")
	set(PROJECT_SOURCE_DIR "..")
	include(FindDoxygen)
endif()

# Cmake doxygen settings
# # # # # # # # # # # # #

option(DOCUMENTATION_CREATE_PDF "Create PDF reference manual" OFF )
option(DOCUMENTATION_INCLUDE_SOURCE_REFERENCE "Include source reference in manual" ON)

# Cmake modules needed
# # # # # # # # # # # #

find_package( HTMLHelp )
find_package( LATEX )

if( HTML_HELP_COMPILER )
  set(DOXYGEN_HTMLHELP YES )
else( HTML_HELP_COMPILER )
  set(DOXYGEN_HTMLHELP NO )
endif( HTML_HELP_COMPILER )

if( LATEX_COMPILER OR PDFLATEX_COMPILER )
  set(DOXYGEN_LATEX YES )
else( LATEX_COMPILER OR PDFLATEX_COMPILER )
  set(DOXYGEN_LATEX NO )
endif( LATEX_COMPILER OR PDFLATEX_COMPILER )

if( PDFLATEX_COMPILER AND DOCUMENTATION_CREATE_PDF )
  set(DOXYGEN_PDFLATEX YES )
else( PDFLATEX_COMPILER AND DOCUMENTATION_CREATE_PDF )
  set(DOXYGEN_PDFLATEX NO )
endif( PDFLATEX_COMPILER AND DOCUMENTATION_CREATE_PDF )

if( DOCUMENTATION_CREATE_PDF )
  set(DOXYGEN_PDF_HYPERLINKS YES )
else( DOCUMENTATION_CREATE_PDF )
  set(DOXYGEN_PDF_HYPERLINKS NO )
endif( DOCUMENTATION_CREATE_PDF )

if( DOT )
  set(HAVE_DOT YES )
else( DOT )
  set(HAVE_DOT NO )
endif( DOT )

# Doxygen settings
# # # # # # # # # #

set(DOXYGEN_LANGUAGE "English" CACHE STRING "Language used by doxygen")
mark_as_advanced(DOXYGEN_LANGUAGE)

set(DOXYGEN_SOURCE_DIR ${PROJECT_SOURCE_DIR}/doc/markdown)

if( DOCUMENTATION_INCLUDE_SOURCE_REFERENCE )
	set(DOXYGEN_SOURCE_DIR ${DOXYGEN_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
endif( DOCUMENTATION_INCLUDE_SOURCE_REFERENCE ) 

set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/doc)

STRING(REGEX REPLACE ";" " " DOXYGEN_INPUT_LIST "${DOXYGEN_SOURCE_DIR}" )

# Generate doxygen.conf
# # # # # # # # # # # #

configure_file(doxygen.conf.in ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf)

# Target for running doxygen
add_custom_target(doc-doxygen
	COMMAND Doxygen::doxygen ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf Doxygen::doxygen
)

# Target for building the whole documentation
add_custom_target(doc
	DEPENDS doc-doxygen
)

if(DOXYGEN_PDFLATEX)
	set(TEX_DIR ${DOXYGEN_OUTPUT_DIR}/latex)
	add_custom_target(doc-pdf
		BYPRODUCTS ${TEX_DIR}/refman.pdf
		COMMAND ${PDFLATEX_COMPILER} refman.tex
		COMMAND ${PDFLATEX_COMPILER} refman.tex
		COMMAND mv ${TEX_DIR}/refman.pdf ${DOXYGEN_OUTPUT_DIR}/doc_${PROJECT_NAME}-${PROJECT_VERSION}.pdf
		WORKING_DIRECTORY ${TEX_DIR}
		DEPENDS doc-doxygen
	)
	add_dependencies(doc doc-pdf)
endif(DOXYGEN_PDFLATEX)
