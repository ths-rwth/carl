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

option(DOCUMENTATION_WITH_STATIC "Create documentation for static members" ON )
option(DOCUMENTATION_WITH_PRIVATE "Create documentation for private members" ON )
option(DOCUMENTATION_WITH_LOCAL "Create documentation for local members" ON )
option(DOCUMENTATION_WITH_INTERNAL "Create documentation for internal features" OFF )

mark_as_advanced(
	DOCUMENTATION_WITH_PRIVATE
	DOCUMENTATION_WITH_STATIC
	DOCUMENTATION_WITH_LOCAL
	DOCUMENTATION_WITH_INTERNAL
 )

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

if( DOCUMENTATION_WITH_PRIVATE )
  set(DOXYGEN_EXTRACT_PRIVATE YES )
else( DOCUMENTATION_WITH_PRIVATE )
  set(DOXYGEN_EXTRACT_PRIVATE NO )
endif( DOCUMENTATION_WITH_PRIVATE )

if( DOCUMENTATION_WITH_STATIC )
  set(DOXYGEN_EXTRACT_STATIC YES )
else( DOCUMENTATION_WITH_STATIC )
  set(DOXYGEN_EXTRACT_STATIC NO )
endif( DOCUMENTATION_WITH_STATIC )

if( DOCUMENTATION_WITH_LOCAL )
  set(DOXYGEN_EXTRACT_LOCAL YES )
else( DOCUMENTATION_WITH_LOCAL )
  set(DOXYGEN_EXTRACT_LOCAL NO )
endif( DOCUMENTATION_WITH_LOCAL )

if( DOCUMENTATION_WITH_INTERNAL )
  set(DOXYGEN_EXTRACT_INTERNAL YES )
else( DOCUMENTATION_WITH_INTERNAL )
  set(DOXYGEN_EXTRACT_INTERNAL NO )
endif( DOCUMENTATION_WITH_INTERNAL )

if( DOT )
  set(HAVE_DOT YES )
else( DOT )
  set(HAVE_DOT NO )
endif( DOT )

# Doxygen settings
# # # # # # # # # #

set(DOXYGEN_LANGUAGE "English" CACHE STRING "Language used by doxygen" )
mark_as_advanced(DOXYGEN_LANGUAGE )

set(DOXYGEN_SOURCE_DIR
	${PROJECT_SOURCE_DIR}/doc/markdown
)
if( DOCUMENTATION_INCLUDE_SOURCE_REFERENCE )
set( DOXYGEN_SOURCE_DIR ${DOXYGEN_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src )
else(DOCUMENTATION_INCLUDE_SOURCE_REFERENCE )

endif( DOCUMENTATION_INCLUDE_SOURCE_REFERENCE ) 



set(DOXYGEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/doc )
STRING(REGEX REPLACE ";" " " DOXYGEN_INPUT_LIST "${DOXYGEN_SOURCE_DIR}" )

# Generate doxygen.conf
# # # # # # # # # # # #

configure_file( doxygen.conf.in ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf )
add_custom_target( doxygen-doc
		           ${DOXYGEN} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf
		           DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/doxygen.conf )

# Add "doc" target depending on whether pdflatex is available or not
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

if( DOXYGEN_PDFLATEX )
  set( TEX_DIR ${CMAKE_CURRENT_BINARY_DIR}/../${DOXYGEN_OUTPUT_DIR}/latex )
  set( PDF_OUTPUT ${TEX_DIR}/refman.pdf )
  add_custom_command( OUTPUT ${PDF_OUTPUT}
					  POST_BUILD
                      COMMAND ${PDFLATEX_COMPILER}
                      ARGS refman.tex
                      WORKING_DIRECTORY ${TEX_DIR}
                      DEPENDS doxygen-doc )
  add_custom_target( doc
                     COMMAND mv -T ${PDF_OUTPUT} ${CMAKE_CURRENT_BINARY_DIR}/doc_${PROJECT_NAME}-${PROJECT_VERSION}.pdf
                     DEPENDS ${PDF_OUTPUT} )
else( DOXYGEN_PDFLATEX )
  add_custom_target( doc 
                     DEPENDS doxygen-doc )
endif( DOXYGEN_PDFLATEX )

