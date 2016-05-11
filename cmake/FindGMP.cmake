# Include dir
find_path(GMP_INCLUDE
	NAMES gmp.h
	DOC "Include directory for GMP"
)
find_path(GMPXX_INCLUDE
	NAMES gmpxx.h
	HINTS ${GMP_INCLUDE}
	DOC "Include directory for GMPXX"
)

# Library files
find_library(GMP_LIBRARY
	NAMES gmp
	HINTS /usr/local/lib
)
get_filename_component(GMP_LIB_PATH ${GMP_LIBRARY} DIRECTORY)
find_library(GMPXX_LIBRARY
	NAMES gmpxx
	HINTS ${GMP_LIB_PATH}
)

if(GMP_INCLUDE AND GMPXX_INCLUDE AND GMP_LIBRARY AND GMPXX_LIBRARY)
   set(GMP_FOUND TRUE)
   set(GMP_INCLUDE_DIR "${GMP_INCLUDE};${GMPXX_INCLUDE}")
   set(GMP_LIBRARIES "${GMP_LIBRARY};${GMPXX_LIBRARY}")
   # Version
	function(GetVersionPart OUTPUT FILENAME DESC)
		file(STRINGS ${FILENAME} RES REGEX "^#define __GNU_MP_${DESC}[ \\t]+.*")
		string(REGEX MATCH "[0-9]+" RES "${RES}")
		set(${OUTPUT} "${RES}" PARENT_SCOPE)
	endfunction()
	GetVersionPart(MAJOR "${GMP_INCLUDE}/gmp.h" "VERSION")
	GetVersionPart(MINOR "${GMP_INCLUDE}/gmp.h" "VERSION_MINOR")
	GetVersionPart(PATCH "${GMP_INCLUDE}/gmp.h" "VERSION_PATCHLEVEL")
	set(GMP_VERSION "${MAJOR}.${MINOR}.${PATCH}")
endif()

# Cleanup
unset(GMP_INCLUDE CACHE)
unset(GMPXX_INCLUDE CACHE)
unset(GMP_LIBRARY CACHE)
unset(GMPXX_LIBRARY CACHE)

mark_as_advanced(
	GMP_FOUND
	GMP_INCLUDE_DIR
	GMP_LIBRARIES
	GMP_VERSION
)
