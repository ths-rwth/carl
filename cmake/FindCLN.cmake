# Include dir
find_path(CLN_INCLUDE_DIR
	NAMES cln/cln.h
	HINTS ${CLN_PKGCONF_INCLUDE_DIRS}
	PATHS
		/usr/include
		/usr/local/include
)

# Library files
find_library(CLN_LIBRARY
	NAMES cln
	HINTS ${CLN_PKGCONF_LIBRARY_DIRS}
	PATHS
		/usr/lib
		/usr/local/lib
)

if(CLN_INCLUDE_DIR AND CLN_LIBRARY)
    set(CLN_FOUND TRUE)

	# Version
	file(STRINGS ${CLN_INCLUDE_DIR}/cln/version.h CLN_VERSION REGEX "^#define[\t ]+CL_VERSION .*")
	string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CLN_VERSION "${CLN_VERSION}")

	if(CLN_FIND_VERSION VERSION_GREATER CLN_VERSION)
		message(WARNING "Required CLN ${CLN_FIND_VERSION} but found only CLN ${CLN_VERSION}.")
		return()
	endif()
endif()

# Cleanup
mark_as_advanced(
    CLN_FOUND
    CLN_INCLUDE_DIR
    CLN_LIBRARY
	CLN_VERSION
)
