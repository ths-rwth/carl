find_program(M4 m4)
if(NOT M4)
	message(FATAL_ERROR "Can not build gmp, missing binary for m4")
	return()
endif()
mark_as_advanced(M4)

ExternalProject_Add(
	gmp
	URL "https://gmplib.org/download/gmp/gmp-${GMP_VERSION}.tar.bz2"
	URL_MD5 86ee6e54ebfc4a90b643a65e402c4048
	BUILD_IN_SOURCE YES
	CONFIGURE_COMMAND ./configure --enable-cxx
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gmp source_dir)

add_imported_library(GMP SHARED "${source_dir}/.libs/libgmp.so" "${source_dir}")
add_imported_library(GMP STATIC "${source_dir}/.libs/libgmp.a" "${source_dir}")
add_imported_library(GMPXX SHARED "${source_dir}/.libs/libgmpxx.so" "${source_dir}")
add_imported_library(GMPXX STATIC "${source_dir}/.libs/libgmpxx.a" "${source_dir}")

add_dependencies(GMP_SHARED gmp)
add_dependencies(GMP_STATIC gmp)
add_dependencies(GMPXX_SHARED gmp)
add_dependencies(GMPXX_STATIC gmp)
add_dependencies(resources GMP_SHARED GMP_STATIC GMPXX_SHARED GMPXX_STATIC)
