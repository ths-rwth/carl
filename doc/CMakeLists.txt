option(DOC_CREATE_HTML "Create HTML documentation" ON)
option(DOC_CREATE_PDF "Create PDF reference manual" ON)

# Check if doxygen is available
if(NOT TARGET Doxygen::doxygen)
	message(STATUS "Doxygen not available, no documentation can be built.")
	return()
endif()

# Check if dot is available
if(TARGET Doxygen::dot)
	set(DOXYGEN_HAVE_DOT YES)
else(TARGET Doxygen::dot)
	set(DOXYGEN_HAVE_DOT NO)
endif(TARGET Doxygen::dot)

# Paths for documentations
set(OUTPUT_DIR ${PROJECT_BINARY_DIR}/doc)
set(CODE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(MANUAL_SOURCE_DIR ${PROJECT_SOURCE_DIR}/doc/manual)

# Collect all sources for proper dependencies
file(GLOB_RECURSE CODE_SOURCES "${CODE_SOURCE_DIR}/**/*.cpp" "${CODE_SOURCE_DIR}/**/*.h" "${CODE_SOURCE_DIR}/**/*.tpp")

# Configure config for HTML documentation
configure_file(apidoc/doxygen.conf.in ${OUTPUT_DIR}/config-apidoc/doxygen.conf)

# Build html documentation
add_custom_command(
	OUTPUT ${OUTPUT_DIR}/html/index.html
	COMMAND Doxygen::doxygen ${OUTPUT_DIR}/config-apidoc/doxygen.conf
	DEPENDS ${OUTPUT_DIR}/config-apidoc/doxygen.conf ${CODE_SOURCES}
)
# Wrapper target to have proper dependencies
add_custom_target(doc-html
	DEPENDS ${OUTPUT_DIR}/html/index.html
)

# umbrella target for the whole documentation
add_custom_target(doc
	DEPENDS doc-html
)

# If pdf manual is enabled, do the same for the manual again
if(DOC_CREATE_PDF)
	find_package(LATEX COMPONENTS PDFLATEX BIBTEX)
	if(LATEX_FOUND)
		file(GLOB_RECURSE MANUAL_SOURCES "${MANUAL_SOURCE_DIR}/**/*")
		# Configure config for Manual
		configure_file(manual/doxygen.conf.in ${OUTPUT_DIR}/config-manual/doxygen.conf)
		# Build latex sources
		add_custom_command(
			OUTPUT ${OUTPUT_DIR}/manual/refman.tex
			COMMAND Doxygen::doxygen ${OUTPUT_DIR}/config-manual/doxygen.conf
			DEPENDS ${OUTPUT_DIR}/config-manual/doxygen.conf ${MANUAL_SOURCES}
		)
		# Wrapper target to have proper dependencies
		add_custom_target(doc-doxygen-manual
			DEPENDS ${OUTPUT_DIR}/manual/refman.tex
		)

		# Build the manual
		set(TEX_DIR ${OUTPUT_DIR}/manual)
		add_custom_target(doc-pdf
			BYPRODUCTS ${TEX_DIR}/refman.pdf
			COMMAND ${PDFLATEX_COMPILER} refman.tex
			COMMAND ${PDFLATEX_COMPILER} refman.tex
			COMMAND mv ${TEX_DIR}/refman.pdf ${OUTPUT_DIR}/doc_${PROJECT_NAME}-${PROJECT_VERSION_LIB}.pdf
			WORKING_DIRECTORY ${TEX_DIR}
			DEPENDS doc-doxygen-manual
		)
		add_dependencies(doc doc-pdf)
	else(LATEX_FOUND)
		message(STATUS "Did not find pdflatex, no pdf manual can be built.")
		set(DOXYGEN_CREATE_PDF NO)
	endif(LATEX_FOUND)
endif(DOC_CREATE_PDF)