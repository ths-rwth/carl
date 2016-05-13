# Include dir
find_path(GMP_INCLUDE_DIR
	NAMES gmp.h
	DOC "Include directory for GMP"
)

# Version
function(GetVersionPart OUTPUT FILENAME DESC)
	file(STRINGS ${FILENAME} RES REGEX "^#define __GNU_MP_${DESC}[ \\t]+.*")
	string(REGEX MATCH "[0-9]+" RES "${RES}")
	set(${OUTPUT} "${RES}" PARENT_SCOPE)
endfunction()
GetVersionPart(MAJOR "${GMP_INCLUDE_DIR}/gmp.h" "VERSION")
GetVersionPart(MINOR "${GMP_INCLUDE_DIR}/gmp.h" "VERSION_MINOR")
GetVersionPart(PATCH "${GMP_INCLUDE_DIR}/gmp.h" "VERSION_PATCHLEVEL")
set(GMP_VERSION "${MAJOR}.${MINOR}.${PATCH}")

if(GMP_FIND_VERSION VERSION_GREATER GMP_VERSION)
	message(WARNING "Required GMP ${GMP_FIND_VERSION} but found only GMP ${GMP_VERSION}.")
	return()
endif()

# Library files
find_library(GMP_LIBRARY
	NAMES gmp
	PATHS /usr/local/lib
)

if(GMP_INCLUDE_DIR AND GMP_LIBRARY)
	set(GMP_FOUND TRUE)
endif()

mark_as_advanced(
	GMP_FOUND
	GMP_INCLUDE_DIR
	GMP_LIBRARY
	GMP_VERSION
)
