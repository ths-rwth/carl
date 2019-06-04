option(DOC_CREATE_DOXYGEN "Create Doxygen HTML" ON)
option(DOC_CREATE_PDF "Create PDF reference manual" OFF)
option(DOC_CREATE_SPHINX "Create Sphinx HTML" OFF)
option(DOC_INCLUDE_SOURCES "Include source reference in manual" ON)

# Cmake modules needed
# # # # # # # # # # # #

if(NOT TARGET Doxygen::doxygen)
	message(STATUS "Doxygen not available, no documentation can be built.")
	return()
endif()

if(TARGET Doxygen::dot)
  set(DOXYGEN_HAVE_DOT YES)
else(TARGET Doxygen::dot)
  set(DOXYGEN_HAVE_DOT NO)
endif(TARGET Doxygen::dot)

# Look for a LaTeX compiler
if(DOC_CREATE_PDF)
	find_package(LATEX COMPONENTS PDFLATEX BIBTEX)

	if(LATEX_FOUND)
		set(DOXYGEN_CREATE_PDF YES)
	else(LATEX_FOUND)
		message(STATUS "Did not find pdflatex, no pdf manual can be built.")
		set(DOXYGEN_CREATE_PDF NO)
	endif(LATEX_FOUND)
endif()

set(DOC_OUTPUT_DIR ${PROJECT_BINARY_DIR}/doc)

set(DOC_SOURCE_DIR ${PROJECT_SOURCE_DIR}/doc/markdown)

if(DOC_INCLUDE_SOURCES)
	set(DOC_SOURCE_DIR ${DOC_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
endif(DOC_INCLUDE_SOURCES)
STRING(REGEX REPLACE ";" " " DOXYGEN_INPUT_LIST "${DOC_SOURCE_DIR}" )


configure_file(doxygen.conf.in ${DOC_OUTPUT_DIR}/config/doxygen.conf)

# Target for running doxygen
add_custom_command(
	OUTPUT ${DOC_OUTPUT_DIR}/xml/index.xml
	COMMAND Doxygen::doxygen ${DOC_OUTPUT_DIR}/config/doxygen.conf
	DEPENDS ${DOC_OUTPUT_DIR}/config/doxygen.conf
)

add_custom_target(doc-doxygen
	DEPENDS ${DOC_OUTPUT_DIR}/xml/index.xml
)

# umbrella target for the whole documentation
add_custom_target(doc
	DEPENDS doc-doxygen
)

if(DOXYGEN_CREATE_PDF)
	set(TEX_DIR ${DOC_OUTPUT_DIR}/latex)
	add_custom_target(doc-pdf
		BYPRODUCTS ${TEX_DIR}/refman.pdf
		COMMAND ${PDFLATEX_COMPILER} refman.tex
		COMMAND ${PDFLATEX_COMPILER} refman.tex
		COMMAND mv ${TEX_DIR}/refman.pdf ${DOC_OUTPUT_DIR}/doc_${PROJECT_NAME}-${PROJECT_VERSION_LIB}.pdf
		WORKING_DIRECTORY ${TEX_DIR}
		DEPENDS doc-doxygen
	)
	add_dependencies(doc doc-pdf)
endif(DOXYGEN_CREATE_PDF)

if(DOC_CREATE_SPHINX)
	configure_file(conf.py.in ${DOC_OUTPUT_DIR}/config/conf.py)
	add_custom_command(
		OUTPUT ${DOC_OUTPUT_DIR}/sphinx/index.html
		COMMAND sphinx-build -a -E -v -b html
			-Dbreathe_default_project=carl -Dbreathe_projects.carl=${DOC_OUTPUT_DIR}/xml
			-c ${DOC_OUTPUT_DIR}/config
			${CMAKE_CURRENT_SOURCE_DIR} ${DOC_OUTPUT_DIR}/sphinx
		WORKING_DIRECTORY ${DOC_OUTPUT_DIR}/config
		DEPENDS ${DOC_OUTPUT_DIR}/xml/index.xml
	)
	add_custom_target(doc-sphinx
		DEPENDS
			doc-doxygen
			${DOC_OUTPUT_DIR}/sphinx/index.html
	)
	add_dependencies(doc doc-sphinx)
endif(DOC_CREATE_SPHINX)
