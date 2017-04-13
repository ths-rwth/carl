if (TARGET GMP_EP)
	get_target_property(GMP_LIB GMP_STATIC IMPORTED_LOCATION)
	set(GMP_LIB_ARG "--with-libgmp=${GMP_LIB}")
endif()

ExternalProject_Add(
    CoCoALib_EP
	URL "http://cocoa.dima.unige.it/cocoalib/tgz/CoCoALib-${COCOA_VERSION}.tgz"
	URL_MD5 19be2e8a20b1cc274dbfd69be7807a95
	BUILD_IN_SOURCE YES
	PATCH_COMMAND patch configure ${CMAKE_SOURCE_DIR}/resources/cocoa/configure_simple.patch
#	PATCH_COMMAND patch --forward configure ${CMAKE_SOURCE_DIR}/resources/cocoa/configure.patch
#		COMMAND patch --forward configuration/gmp-check-cxxflags.sh ${CMAKE_SOURCE_DIR}/resources/cocoa/configuration/gmp-check-cxxflags.sh.patch
#		COMMAND patch --forward configuration/gmp-cxx-flags.sh ${CMAKE_SOURCE_DIR}/resources/cocoa/configuration/gmp-cxx-flags.sh.patch
#		COMMAND patch --forward configuration/gmp-find-hdr.sh ${CMAKE_SOURCE_DIR}/resources/cocoa/configuration/gmp-find-hdr.sh.patch
#		COMMAND patch --forward configuration/gmp-try-default.sh ${CMAKE_SOURCE_DIR}/resources/cocoa/configuration/gmp-try-default.sh.patch
#		COMMAND patch --forward configuration/gmp-version.sh ${CMAKE_SOURCE_DIR}/resources/cocoa/configuration/gmp-version.sh.patch
#		COMMAND cp ${CMAKE_SOURCE_DIR}/resources/cocoa/configuration/fpic-ldflag.sh configuration/
	CONFIGURE_COMMAND ./configure --threadsafe-hack ${GMP_LIB_ARG} --with-cxxflags=-Wno-deprecated-declarations --with-cxxflags=-fPIC
	BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} cocoalib > /dev/null
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(CoCoALib_EP SOURCE_DIR)

add_imported_library(COCOA SHARED "${SOURCE_DIR}/lib/libcocoa${STATIC_EXT}" "${SOURCE_DIR}/include")
add_imported_library(COCOA STATIC "${SOURCE_DIR}/lib/libcocoa${STATIC_EXT}" "${SOURCE_DIR}/include")

add_dependencies(CoCoALib_EP GMP_STATIC)
add_dependencies(COCOA_SHARED CoCoALib_EP)
add_dependencies(COCOA_STATIC CoCoALib_EP)
add_dependencies(resources COCOA_SHARED COCOA_STATIC)
