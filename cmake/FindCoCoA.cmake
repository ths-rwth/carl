if(NOT FORCE_SHIPPED_RESOURCES)
	find_path(COCOA_INCLUDE_DIR
	NAMES CoCoA/library.H
	PATHS
		/usr/include/
		/usr/local/include/
	DOC "Include directory for CoCoA"
	)

	#find_library(COCOA_LIBRARY
	#NAMES cocoa
	#PATHS
	#	/usr/lib 
	#	/usr/local/lib 
	#)

	find_library(COCOA_SHARED NAMES cocoa${DYNAMIC_EXT})
	find_library(COCOA_STATIC NAMES cocoa${STATIC_EXT})
endif()

set(COCOA_FOUND_SYSTEM FALSE)
if(COCOA_INCLUDE_DIR AND COCOA_SHARED AND COCOA_STATIC)
	function(GetVersion OUTPUT FILENAME)
		file(STRINGS ${FILENAME} RES REGEX "CoCoALib version .*")
		string(REGEX MATCH "[0-9]+\.[0-9]+" RES "${RES}")
		set(${OUTPUT} "${RES}" PARENT_SCOPE)
	endfunction()
	GetVersion(COCOA_VERSION_SYSTEM "${COCOA_INCLUDE_DIR}/CoCoA/library.H")

	if(COCOA_FIND_VERSION VERSION_GREATER COCOA_VERSION_SYSTEM)
		message(WARNING "Required CoCoA ${COCOA_FIND_VERSION} but found only CoCoA ${COCOA_VERSION_SYSTEM}.")
	else()
		set(COCOA_FOUND_SYSTEM TRUE)
		set(COCOA_VERSION COCOA_VERSION_SYSTEM)
	endif()
endif()

if(NOT COCOA_FOUND_SYSTEM)
	get_target_property(GMP_LIB GMP_STATIC IMPORTED_LOCATION)
	set(GMP_LIB_ARG "--with-libgmp=${GMP_LIB}")

	ExternalProject_Add(
		CoCoALib-EP
		URL "https://cocoa.altervista.org/cocoalib/tgz/CoCoALib-${COCOA_VERSION}.tgz"
		URL_HASH SHA256=${COCOA_TGZHASH}
		DOWNLOAD_NO_PROGRESS 1
		BUILD_IN_SOURCE YES
		PATCH_COMMAND sh ${CMAKE_SOURCE_DIR}/cmake/patches/cocoa_patch_print_errors.sh
		COMMAND patch <SOURCE_DIR>/configuration/shell-fns.sh ${CMAKE_SOURCE_DIR}/cmake/patches/cocoa_shell-fns.sh.patch
		CONFIGURE_COMMAND ./configure --prefix=<INSTALL_DIR> --threadsafe-hack ${GMP_LIB_ARG}
		BUILD_COMMAND make library
		INSTALL_COMMAND ${CMAKE_COMMAND} -E touch <SOURCE_DIR>/examples/index.html
		COMMAND ${CMAKE_COMMAND} -E make_directory <INSTALL_DIR>/include
		COMMAND ${CMAKE_COMMAND} -E make_directory <INSTALL_DIR>/lib
		COMMAND ${CMAKE_COMMAND} -E remove_directory <INSTALL_DIR>/include/CoCoA-${COCOA_VERSION}
		COMMAND ${CMAKE_MAKE_PROGRAM} install
		BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/resources/src/CoCoALib-EP/lib/libcocoa${STATIC_EXT}
	)

	ExternalProject_Get_Property(CoCoALib-EP INSTALL_DIR)

	add_imported_library(COCOA SHARED "${INSTALL_DIR}/lib/libcocoa${STATIC_EXT}" "${INSTALL_DIR}/include")
	add_imported_library(COCOA STATIC "${INSTALL_DIR}/lib/libcocoa${STATIC_EXT}" "${INSTALL_DIR}/include")

	add_dependencies(CoCoALib-EP GMP_STATIC)
	add_dependencies(COCOA_SHARED CoCoALib-EP GMPXX_SHARED)
	add_dependencies(COCOA_STATIC CoCoALib-EP GMP_STATIC)
endif()

set(COCOA_FOUND TRUE)

mark_as_advanced(COCOA_VERSION COCOA_INCLUDE_DIR COCOA_SHARED COCOA_STATIC COCOA_FOUND)
