set( GINAC_FOUND FALSE )

find_path( GINAC_INCLUDE_DIR ginac.h
           /usr/include/ginac
           /usr/local/include/ginac
           /opt/local/include/ginac
           $ENV{UNITTESTXX_PATH}/src
           $ENV{UNITTESTXX_INCLUDE_PATH} )

find_library( GINAC_LIBRARY NAMES ginac PATHS 
              /usr/lib 
              /usr/local/lib 
              /opt/local/lib 
              $ENV{UNITTESTXX_PATH} 
              ENV{UNITTESTXX_LIBRARY_PATH} )

if( GINAC_INCLUDE_DIR AND GINAC_LIBRARY )
        SET( GINAC_FOUND TRUE )
ENDIF (GINAC_INCLUDE_DIR AND GINAC_LIBRARY)

IF (GINAC_FOUND)
   IF (NOT GiNaC_FIND_QUIETLY)
      MESSAGE(STATUS "Found GiNaC: ${GINAC_LIBRARY}")
   ENDIF (NOT GiNaC_FIND_QUIETLY)
ELSE (GINAC_FOUND)
   IF (GiNaC_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find GiNaC")
   ENDIF (GiNaC_FIND_REQUIRED)
ENDIF (GINAC_FOUND)

MARK_AS_ADVANCED (	GINAC_INCLUDE_DIR
					GINAC_LIBRARY
				 )