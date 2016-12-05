find_program(M4 m4)
if(NOT M4)
	message(FATAL_ERROR "Can not build gmp, missing binary for m4")
endif()
mark_as_advanced(M4)

ExternalProject_Add(
	GMP_EP
	URL "https://gmplib.org/download/gmp/gmp-${GMP_VERSION}.tar.bz2"
	URL_MD5 86ee6e54ebfc4a90b643a65e402c4048
	BUILD_IN_SOURCE YES
	CONFIGURE_COMMAND ./configure --enable-cxx --prefix=<INSTALL_DIR>
)

ExternalProject_Get_Property(GMP_EP INSTALL_DIR)

add_imported_library(GMP SHARED "${INSTALL_DIR}/lib/libgmp.${DYNAMIC_EXT}" "${INSTALL_DIR}")
add_imported_library(GMP STATIC "${INSTALL_DIR}/lib/libgmp.${STATIC_EXT}" "${INSTALL_DIR}")
add_imported_library(GMPXX SHARED "${INSTALL_DIR}/lib/libgmpxx.${DYNAMIC_EXT}" "${INSTALL_DIR}")
add_imported_library(GMPXX STATIC "${INSTALL_DIR}/lib/libgmpxx.${STATIC_EXT}" "${INSTALL_DIR}")

add_dependencies(GMP_SHARED GMP_EP)
add_dependencies(GMP_STATIC GMP_EP)
add_dependencies(GMPXX_SHARED GMP_EP)
add_dependencies(GMPXX_STATIC GMP_EP)
add_dependencies(resources GMP_SHARED GMP_STATIC GMPXX_SHARED GMPXX_STATIC)
