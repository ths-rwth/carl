
# Add all targets to the build-tree export set
export(TARGETS ${CARL_TARGETS} FILE "${PROJECT_BINARY_DIR}/carlTargets.cmake")

message(STATUS "Registered with cmake")
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE carl)

set(DEP_TARGETS "")
if(USE_CLN_NUMBERS)
	export_target(DEP_TARGETS CLN_SHARED)
	export_target(DEP_TARGETS CLN_STATIC GMP_STATIC)
endif()
if(USE_GINAC)
	export_target(DEP_TARGETS GINAC_SHARED)
	export_target(DEP_TARGETS GINAC_STATIC CLN_STATIC)
endif()
export_target(DEP_TARGETS GMP_SHARED)
export_target(DEP_TARGETS GMP_STATIC)
export_target(DEP_TARGETS GMPXX_SHARED)
export_target(DEP_TARGETS GMPXX_STATIC GMP_STATIC)
export_target(DEP_TARGETS EIGEN3)
export_target(DEP_TARGETS GTESTCORE_STATIC)
export_target(DEP_TARGETS GTESTMAIN_STATIC)

include(CMakePackageConfigHelpers)

set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src")
configure_package_config_file(
	cmake/carlConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/carlConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR #SYSCONFIG_INSTALL_DIR
)
