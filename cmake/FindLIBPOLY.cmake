# - Try to find libpoly
# Once done this will define
#  LIBPOLY_FOUND - System has libpoly
#  LIBPOLY_INCLUDE_DIRS - The libpoly include directories
#  LIBPOLY_LIBRARIES, LIBPOLYXX_LIBRARIES - The libraries needed to use libpoly

# Include dir
find_path(LIBPOLY_INCLUDE_DIR
	NAMES poly/poly.h
	PATHS
		/usr/include/
		/usr/local/include/
	DOC "Include directory for LibPoly"
)

find_library(LIBPOLY_LIBRARIES
	NAMES poly
	PATHS
		/usr/lib 
		/usr/local/lib 
)

find_library(LIBPOLYXX_LIBRARIES
	NAMES polyxx
	PATHS
		/usr/lib 
		/usr/local/lib 
)

set(LIBPOLY_FOUND FALSE)

if(LIBPOLY_INCLUDE_DIR AND LIBPOLY_LIBRARIES and LIBPOLYXX_LIBRARIES)
	set(LIBPOLY_FOUND TRUE)

file(STRINGS ${LIBPOLY_INCLUDE_DIR}/poly/version.h LIBPOLY_VERSION
REGEX "^#define[\t ]+LIBPOLY_VERSION [0-9+]+"
)
string(REGEX MATCH "[0-9.]+" LIBPOLY_VERSION "${LIBPOLY_VERSION}")

	if (LIBPOLY_FIND_VERSION)
	if(LIBPOLY_VERSION VERSION_LESS LIBPOLY_FIND_VERSION)
		message(VERBOSE "System version for LIBPOLY has incompatible \
	version: required ${LIBPOLY_FIND_VERSION} but found ${LIBPOLY_VERSION}")
		set(LIBPOLY_FOUND FALSE)
	endif()
	endif()

endif()












