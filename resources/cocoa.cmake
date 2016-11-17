ExternalProject_Add(
    CoCoALib
	URL "http://cocoa.dima.unige.it/cocoalib/tgz/CoCoALib-${COCOA_VERSION}.tgz"
	URL_MD5 19be2e8a20b1cc274dbfd69be7807a95
	BUILD_IN_SOURCE YES
	CONFIGURE_COMMAND ./configure --threadsafe-hack
	PATCH_COMMAND patch configure ${CMAKE_SOURCE_DIR}/resources/cocoa_1.patch
	BUILD_COMMAND make cocoalib
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(CoCoALib source_dir)

add_imported_library(COCOA SHARED "${source_dir}/lib/libcocoa.a" "${source_dir}/include")
add_imported_library(COCOA STATIC "${source_dir}/lib/libcocoa.a" "${source_dir}/include")

add_dependencies(resources CoCoALib)
