set(gmp_VERSION "6.1.0")

find_program(M4 m4)
if(NOT M4)
	message(ERROR "Can not build gmp, missing binary for m4")
endif()

ExternalProject_Add(
    gmp
	URL "https://gmplib.org/download/gmp/gmp-${gmp_VERSION}.tar.bz2"
	URL_MD5 86ee6e54ebfc4a90b643a65e402c4048
	BUILD_IN_SOURCE YES
	CONFIGURE_COMMAND ./configure --enable-cxx
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(gmp source_dir)
set(GMP_INCLUDE_DIR ${source_dir}/include)
set(GMP_LIBRARIES_DYNAMIC "${source_dir}/.libs/libgmpxx.so;${source_dir}/.libs/libgmp.so")
set(GMP_LIBRARIES_STATIC "${source_dir}/.libs/libgmpxx.a;${source_dir}/.libs/libgmp.a")

add_dependencies(resources gmp)
