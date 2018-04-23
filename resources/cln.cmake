find_program(AUTORECONF autoreconf)
if(NOT AUTORECONF)
	message(SEND_ERROR "Can not build cln, missing binary for autoreconf")
endif()

string(REPLACE "." "-" CLN_TAG ${CLN_VERSION})

ExternalProject_Add(
    CLN-EP
    GIT_REPOSITORY "git://www.ginac.de/cln.git"
	GIT_TAG "cln_${CLN_TAG}"
	UPDATE_COMMAND ""
	CONFIGURE_COMMAND cd <SOURCE_DIR> && autoreconf -iv
	COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
)

ExternalProject_Get_Property(CLN-EP INSTALL_DIR)

add_imported_library(CLN SHARED "${INSTALL_DIR}/lib/libcln${DYNAMIC_EXT}" "${INSTALL_DIR}/include")
add_imported_library(CLN STATIC "${INSTALL_DIR}/lib/libcln${STATIC_EXT}" "${INSTALL_DIR}/include")

add_dependencies(CLN_SHARED CLN-EP)
add_dependencies(CLN_STATIC CLN-EP)
add_dependencies(resources CLN_SHARED CLN_STATIC)

mark_as_advanced(AUTORECONF)
