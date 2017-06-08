
# Add all targets to the build-tree export set
export(EXPORT carl_Targets FILE "${PROJECT_BINARY_DIR}/carlExport/carlTargets.cmake")

message(STATUS "Registered with cmake")
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
if(EXPORT_TO_CMAKE)
	export(PACKAGE carl)
endif()

set(DEP_TARGETS "")
if(USE_CLN_NUMBERS)
	export_target(DEP_TARGETS CLN_SHARED)
	export_target(DEP_TARGETS CLN_STATIC GMP_STATIC)
endif()
if(USE_GINAC)
	export_target(DEP_TARGETS GINAC_SHARED)
	export_target(DEP_TARGETS GINAC_STATIC CLN_STATIC)
endif()
if(USE_COCOA)
	export_target(DEP_TARGETS COCOA_STATIC)
endif()
export_target(DEP_TARGETS GMP_SHARED)
export_target(DEP_TARGETS GMP_STATIC)
export_target(DEP_TARGETS GMPXX_SHARED)
export_target(DEP_TARGETS GMPXX_STATIC GMP_STATIC)
export_target(DEP_TARGETS EIGEN3)

export_target_recursive(DEP_TARGETS Boost_SHARED INTERFACE_LINK_LIBRARIES)
export_target_recursive(DEP_TARGETS Boost_STATIC INTERFACE_LINK_LIBRARIES)

set(EXP_OPTIONS "")
foreach(option ${EXPORTED_OPTIONS})
	set(EXP_OPTIONS "${EXP_OPTIONS}\nset(CARL_${option} \"${${option}}\")")
endforeach()

include(CMakePackageConfigHelpers)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/carlExport/carlConfigVersion.cmake
	 VERSION 0.1.0
	 COMPATIBILITY SameMajorVersion )

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src") # TODO looks not correct, this would be hypro_INCLUDE_DIR

configure_package_config_file(
	cmake/carlConfig.cmake.in
	${PROJECT_BINARY_DIR}/carlExport/carlConfig.cmake
	INSTALL_DESTINATION ${PROJECT_BINARY_DIR}
	PATH_VARS PROJECT_SOURCE_DIR #SYSCONFIG_INSTALL_DIR
)
 # ... for the install tree
file(RELATIVE_PATH REL_INCLUDE_DIR "${CMAKE_INSTALL_DIR}" "${INCLUDE_INSTALL_DIR}")
set(CONF_INCLUDE_DIRS "\${carl_CMAKE_DIR}/${REL_INCLUDE_DIR}")

configure_package_config_file(
	cmake/carlConfig.cmake.in
	${PROJECT_BINARY_DIR}/carlInstall/carlConfig.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_DIR}
	PATH_VARS INCLUDE_INSTALL_DIR #SYSCONFIG_INSTALL_DIR
)
