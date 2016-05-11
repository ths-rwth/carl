# Include dir
find_path(CLN_INCLUDE_DIR
  NAMES cln/cln.h
  HINTS ${CLN_PKGCONF_INCLUDE_DIRS}
)

# Version
file(STRINGS ${CLN_INCLUDE_DIR}/cln/version.h CLN_VERSION REGEX "^#define[\t ]+CL_VERSION .*")
string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CLN_VERSION "${CLN_VERSION}")

# Library files
find_library(CLN_LIBRARIES
  NAMES cln
  HINTS ${CLN_PKGCONF_LIBRARY_DIRS}
)
if(CLN_INCLUDE_DIR AND CLN_LIBRARIES)
    set(CLN_FOUND TRUE)
endif()

# Cleanup
mark_as_advanced(
    CLN_FOUND
    CLN_INCLUDE_DIR
    CLN_LIBRARIES
	CLN_VERSION
)
