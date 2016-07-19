find_program(AUTORECONF autoreconf)
if(NOT AUTORECONF)
	message(ERROR "Can not build cln, missing binary for autoreconf")
endif()

string(REPLACE "." "-" CLN_TAG ${CLN_VERSION})

ExternalProject_Add(
    cln
    GIT_REPOSITORY "git://www.ginac.de/cln.git"
	GIT_TAG "cln_${CLN_TAG}"
	BUILD_IN_SOURCE 1
	CONFIGURE_COMMAND ${AUTORECONF} -iv COMMAND ./configure --prefix=<INSTALL_DIR>
)

ExternalProject_Get_Property(cln INSTALL_DIR)

add_imported_library(CLN SHARED "${INSTALL_DIR}/lib/libcln.so" "${INSTALL_DIR}/include")
add_imported_library(CLN STATIC "${INSTALL_DIR}/lib/libcln.a" "${INSTALL_DIR}/include")

add_dependencies(resources cln)

mark_as_advanced(AUTORECONF)
