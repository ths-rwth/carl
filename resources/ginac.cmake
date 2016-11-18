find_program(AUTORECONF autoreconf)
if(NOT AUTORECONF)
	message(ERROR "Can not build GiNaC, missing binary for autoreconf")
	return()
endif()

find_program(PYTHON2 python2)
if(NOT PYTHON2)
	message(ERROR "Can not build GiNaC, missing binary for python2")
	return()
endif()

string(REPLACE "." "-" GINAC_TAG ${GINAC_VERSION})

ExternalProject_Add(
    GiNaC
    GIT_REPOSITORY "git://www.ginac.de/ginac.git"
	GIT_TAG "release_${GINAC_TAG}"
	CONFIGURE_COMMAND ${AUTORECONF} -iv <SOURCE_DIR> COMMAND PYTHON=${PYTHON2} <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
)

ExternalProject_Get_Property(GiNaC INSTALL_DIR)

add_imported_library(GINAC SHARED "${INSTALL_DIR}/lib/libginac.so" "${INSTALL_DIR}/include")
add_imported_library(GINAC STATIC "${INSTALL_DIR}/lib/libginac.a" "${INSTALL_DIR}/include")

add_dependencies(GiNaC CLN_SHARED CLN_STATIC)
add_dependencies(GINAC_SHARED GiNaC)
add_dependencies(GINAC_STATIC GiNaC)
add_dependencies(resources GINAC_SHARED GINAC_STATIC)

mark_as_advanced(AUTORECONF)
