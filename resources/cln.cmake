set(cln_VERSION "1-3-4")

find_program(AUTORECONF autoreconf)
if(NOT AUTORECONF)
	message(ERROR "Could not build cln, missing binary for autoreconf")
endif()

ExternalProject_Add(
    cln
    GIT_REPOSITORY "git://www.ginac.de/cln.git"
	GIT_TAG "cln_${cln_VERSION}"
	BUILD_IN_SOURCE YES
	CONFIGURE_COMMAND ${AUTORECONF} -iv COMMAND ./configure
	BUILD_COMMAND make
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(cln source_dir)
set(CLN_INCLUDE_DIR "${source_dir}/include")
set(CLN_LIBRARIES_DYNAMIC "${source_dir}/src/.libs/libcln.so")
set(CLN_LIBRARIES_STATIC "${source_dir}/src/.libs/libcln.a")

add_dependencies(resources cln)
