
# Include dir
find_path(CLN_INCLUDE_DIR
  NAMES cln/cln.h
  HINTS ${CLN_PKGCONF_INCLUDE_DIRS}
)


# Finally the library itself
find_library(CLN_LIBRARIES
  NAMES cln
  HINTS ${CLN_PKGCONF_LIBRARY_DIRS}
)
if(CLN_INCLUDE_DIR AND CLN_LIBRARIES)
    set(CLN_FOUND TRUE)
else()
    if (CLN_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find CLN")
    endif()
endif()

MARK_AS_ADVANCED(
    CLN_FOUND
    CLN_INCLUDE_DIR
    CLN_LIBRARIES
)
