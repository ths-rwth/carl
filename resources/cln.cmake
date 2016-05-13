find_program(AUTORECONF autoreconf)
if(NOT AUTORECONF)
	message(ERROR "Can not build cln, missing binary for autoreconf")
endif()

string(REPLACE "." "-" CLN_TAG ${CLN_VERSION})

ExternalProject_Add(
    cln
    GIT_REPOSITORY "git://www.ginac.de/cln.git"
	GIT_TAG "cln_${CLN_TAG}"
	BUILD_IN_SOURCE YES
	CONFIGURE_COMMAND ${AUTORECONF} -iv COMMAND ./configure
	BUILD_COMMAND make
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(cln source_dir)

add_imported_library(CLN SHARED "${source_dir}/src/.libs/libcln.so" "${source_dir}/include")
add_imported_library(CLN STATIC "${source_dir}/src/.libs/libcln.a" "${source_dir}/include")

add_dependencies(resources cln)

mark_as_advanced(AUTORECONF)
