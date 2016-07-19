find_program(AUTORECONF autoreconf)
if(NOT AUTORECONF)
	message(ERROR "Can not build GiNaC, missing binary for autoreconf")
endif()

find_program(PYTHON2 python2)
if(NOT PYTHON2)
	message(ERROR "Can not build GiNaC, missing binary for python2")
endif()

string(REPLACE "." "-" GINAC_TAG ${GINAC_VERSION})

ExternalProject_Add(
    ginac
    GIT_REPOSITORY "git://www.ginac.de/ginac.git"
	GIT_TAG "release_${GINAC_TAG}"
	CONFIGURE_COMMAND ${AUTORECONF} -iv COMMAND PYTHON=${PYTHON2} ./configure --prefix=<INSTALL_DIR>
)

ExternalProject_Get_Property(ginac INSTALL_DIR)

add_imported_library(GINAC SHARED "${source_dir}/lib/libginac.so" "${INSTALL_DIR}/include")
add_imported_library(GINAC STATIC "${source_dir}/lib/libginac.a" "${INSTALL_DIR}/include")

add_dependencies(resources ginac)

mark_as_advanced(AUTORECONF)
