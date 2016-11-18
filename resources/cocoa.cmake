ExternalProject_Add(
    CoCoALib
	URL "http://cocoa.dima.unige.it/cocoalib/tgz/CoCoALib-${COCOA_VERSION}.tgz"
	URL_MD5 19be2e8a20b1cc274dbfd69be7807a95
	BUILD_IN_SOURCE YES
	CONFIGURE_COMMAND ./configure --no-readline --threadsafe-hack --prefix=<INSTALL_DIR>
#	PATCH_COMMAND patch configure ${CMAKE_SOURCE_DIR}/resources/cocoa/configure_simple.patch
	PATCH_COMMAND patch --forward configure ${CMAKE_SOURCE_DIR}/resources/cocoa/configure.patch
		COMMAND patch --forward configuration/gmp-check-cxxflags.sh ${CMAKE_SOURCE_DIR}/resources/cocoa/gmp-check-cxxflags.sh.patch
		COMMAND patch --forward configuration/gmp-cxx-flags.sh ${CMAKE_SOURCE_DIR}/resources/cocoa/gmp-cxx-flags.sh.patch
		COMMAND patch --forward configuration/gmp-find-hdr.sh ${CMAKE_SOURCE_DIR}/resources/cocoa/gmp-find-hdr.sh.patch
		COMMAND patch --forward configuration/gmp-try-default.sh ${CMAKE_SOURCE_DIR}/resources/cocoa/gmp-try-default.sh.patch
		COMMAND patch --forward configuration/gmp-version.sh ${CMAKE_SOURCE_DIR}/resources/cocoa/gmp-version.sh.patch
	BUILD_COMMAND make cocoalib
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(CoCoALib source_dir)

add_imported_library(COCOA SHARED "${source_dir}/lib/libcocoa.a" "${source_dir}/include")
add_imported_library(COCOA STATIC "${source_dir}/lib/libcocoa.a" "${source_dir}/include")

add_dependencies(resources CoCoALib)
