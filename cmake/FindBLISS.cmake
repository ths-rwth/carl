# Include dir
find_path(BLISS_INCLUDE_DIR
	NAMES bliss/bliss_C.h
	PATHS
		/usr/include
		/usr/local/include
)

# Library files
find_library(BLISS_LIBRARY
	NAMES bliss
	PATHS
		/usr/lib
		/usr/local/lib
)

if(BLISS_INCLUDE_DIR AND BLISS_LIBRARY)
    set(BLISS_FOUND TRUE)

	# Version
	file(STRINGS ${BLISS_INCLUDE_DIR}/bliss/defs.hh BLISS_VERSION REGEX "const version = \".*\";")
	string(REGEX MATCH "[0-9]+\\.[0-9]+" BLISS_VERSION "${BLISS_VERSION}")

	if(BLISS_FIND_VERSION VERSION_GREATER BLISS_VERSION)
		message(WARNING "Required bliss ${BLISS_FIND_VERSION} but found only bliss ${BLISS_VERSION}.")
		return()
	endif()
endif()

# Cleanup
mark_as_advanced(
    BLISS_FOUND
    BLISS_INCLUDE_DIR
    BLISS_LIBRARY
	BLISS_VERSION
)
