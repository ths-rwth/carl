ExternalProject_Add(
	LCOV_EP
	GIT_REPOSITORY "https://github.com/linux-test-project/lcov"
	GIT_TAG "v1.12"
	BUILD_IN_SOURCE YES
	CONFIGURE_COMMAND ""
	INSTALL_COMMAND ""
)

ExternalProject_Get_Property(LCOV_EP SOURCE_DIR)

add_executable(lcov IMPORTED)
set_target_properties(lcov PROPERTIES IMPORTED_LOCATION  "${SOURCE_DIR}/bin/lcov")

get_target_property(foo lcov IMPORTED_LOCATION)
message("Got lcov at ${foo}")

#add_imported_library(GMP SHARED "${INSTALL_DIR}/lib/libgmp.so" "${INSTALL_DIR}")
#add_imported_library(GMP STATIC "${INSTALL_DIR}/lib/libgmp.a" "${INSTALL_DIR}")
#add_imported_library(GMPXX SHARED "${INSTALL_DIR}/lib/libgmpxx.so" "${INSTALL_DIR}")
#add_imported_library(GMPXX STATIC "${INSTALL_DIR}/lib/libgmpxx.a" "${INSTALL_DIR}")
#
#add_dependencies(GMP_SHARED GMP_EP)
#add_dependencies(GMP_STATIC GMP_EP)
#add_dependencies(GMPXX_SHARED GMP_EP)
#add_dependencies(GMPXX_STATIC GMP_EP)
#add_dependencies(resources GMP_SHARED GMP_STATIC GMPXX_SHARED GMPXX_STATIC)
