
# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties.
# B. Options.
# C. Find system tools and libraries.
# D. Write information files.
# E. Invoke subdirectories.
# F. Export Compile Information
# G. CPack
# # # # # # # # # # # # # # # # # # # # # #

cmake_minimum_required (VERSION 3.0 FATAL_ERROR)
if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} VERSION_GREATER 3.0)
	cmake_policy(VERSION 3.1)
endif()

# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties
# # # # # # # # # # # # # # # # # # # # # #
project(carl CXX)

set( PROJECT_FULLNAME "carl")
set( PROJECT_DESCRIPTION "Computer ARithmetic Library")
set( carl_MAJORVERSION 1)
set( carl_YEARVERSION 17)
set( carl_MONTHVERSION 04)
set( carl_MAINTENANCEVERSION 0)
if(carl_MAINTENANCEVERSION GREATER 0)
	set(carl_VERSION "${carl_MAJORVERSION}.${carl_YEARVERSION}.${carl_MONTHVERSION}.${carl_MAINTENANCEVERSION}")
else()
	set(carl_VERSION "${carl_MAJORVERSION}.${carl_YEARVERSION}.${carl_MONTHVERSION}")
endif()
set( PROJECT_VERSION "${carl_VERSION}")
set( carl_NAME "CArL" )
set( carl_DESCRIPTION ${PROJECT_DESCRIPTION} )

# path to find own modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
# for cmake 2.8.7
include(FindPackageHandleStandardArgs)

# Include own macros.
include( carlmacros )

# # # # # # # # # # # # # # # # # # # # # #
# B. Options
# # # # # # # # # # # # # # # # # # # # # #

# options
option( DEVELOPER "Compile with extra warnings" OFF )
option( ALLWARNINGS "Compile with even more warnings" OFF )
option( LOGGING "Enable logging within the carl library" OFF )
export_option(LOGGING)
option( LOGGING_CARL "Compile with the logging facility of the carl library" OFF )
option( LOGGING_DISABLE_INEFFICIENT "Disable log messages about inefficient methods" OFF )
option( FORCE_SHIPPED_RESOURCES "Do not look in system for resources which are included" OFF )
export_option(FORCE_SHIPPED_RESOURCES)
option( USE_GINAC "Compile with testing with the runtime and result comparisons of carl to ginac" OFF )
export_option(USE_GINAC)
option( COMPARE_WITH_Z3 "Compile benchmarks that compare to z3" OFF )
option( USE_Z3_NUMBERS "Make z3 rationals available in carl" OFF )
option( USE_CLN_NUMBERS "Make cln rationals and integers available in carl" OFF )
export_option(USE_CLN_NUMBERS)
option( USE_COCOA "Use CoCoALib" OFF )
export_option(USE_COCOA)
OPTION( USE_MPFR_FLOAT "Use the mpfr implementation of floating point numbers." OFF )
export_option(USE_MPFR_FLOAT)
option( USE_COTIRE "Use cotire to generate and use precompiled headers" OFF )
option( BUILD_STATIC "Build the static library as well" OFF )
export_option(BUILD_STATIC)
option( THREAD_SAFE "Use mutexing to assure thread safety" OFF )
export_option(THREAD_SAFE)
option( PRUNE_MONOMIAL_POOL "Prune monomial pool" ON )
option( COVERAGE "Enable collection of coverage statistics" OFF )
option( EXPORT_TO_CMAKE "Export the project to CMake for easy inclusion" ON)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "DEBUG" "RELEASE")

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RELEASE" CACHE STRING "Build type")
endif()

set(CARL_LIBRARIES_DIR "${PROJECT_BINARY_DIR}/lib")

# Offer the user the choice of overriding the installation directories
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for header files" )
set(LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
#set(SYSCONFIG_INSTALL_DIR etc/carl/  CACHE PATH "Installation for system configuration files)
set(BIN_INSTALL_DIR lib/ CACHE PATH "Installation directory for executables")


if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/carl)
endif()
set(CMAKE_INSTALL_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH  "Installation directory for CMake files")


foreach(p LIB BIN INCLUDE CMAKE)
  set(var ${p}_INSTALL_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# path to put in the executables after building.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin CACHE PATH "Directory for built executables")

include(compiler-options)

set(DYNAMIC_EXT ".so")
set(STATIC_EXT ".a")
if(APPLE)
	set(DYNAMIC_EXT ".dylib")
	set(STATIC_EXT ".a")
elseif (WIN32)
	set(DYNAMIC_EXT ".dll")
	set(STATIC_EXT ".lib")
endif()

if(BUILD_STATIC)
	message(STATUS "Building static: yes")
	if (LOGGING)
	message(WARNING "A static build with logging enabled will probably trigger a segfault!")
	endif()
else()
	message(STATUS "Building static: no")
endif()


if(USE_COTIRE)
	# We use cotire, simply include it
	message(STATUS "using cotire")
	include(cotire)
else()
	# We do not use cotire, create dummy function.
	message(STATUS "not using cotire")
	function(cotire)
	endfunction(cotire)
endif()


# # # # # # # # # # # # # # # # # # # # # #
# C. Find system tools and libraries.
#
# 1. Required libraries for core.
# 2. Optional libraries for core.
# 3. Development and debug libraries.
# # # # # # # # # # # # # # # # # # # # # #

# # # # # # # # # # # # # # # # # # # # # #
# C.1. Required libraries for core.
# # # # # # # # # # # # # # # # # # # # # #

include(resources/resources.cmake)

# we only search for this libraries in the system, if we do not force using the shipped resources.

enable_testing()

#dl must be linked LAST as this is required for the stacktrace in util/platform.h
if (NOT WIN32)
	list(APPEND carl_LIBRARIES_DYNAMIC pthread dl)
endif()

# # # # # # # # # # # # # # # # # # # # # #
# D. Writen information files
# # # # # # # # # # # # # # # # # # # # # #

# Getting git info (To be added later)
#include(GetGitRevisionDescription)
#get_git_head_revision(GIT_REFSPEC GIT_SHA1)

include(clang-tidy)
include(coverage)

# # # # # # # # # # # # # # # # # # # # # #
# E. Invoke subdirectories.
# # # # # # # # # # # # # # # # # # # # # #

add_subdirectory(src)

# # # # # # # # # # # # # # # # # # # # # #
# F. Export Compile Information
# # # # # # # # # # # # # # # # # # # # # #

include(export)

#write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/carlConfigVersion.cmake
#	 VERSION 0.1.0
# COMPATIBILITY SameMajorVersion )
# ... for the install tree
#file(RELATIVE_PATH REL_INCLUDE_DIR "${CMAKE_INSTALL_DIR}" "${INCLUDE_INSTALL_DIR}")
#set(CONF_INCLUDE_DIRS "\${carl_CMAKE_DIR}/${REL_INCLUDE_DIR}")
#configure_package_config_file(cmake/carlConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/carlConfig.cmake
#							  INSTALL_DESTINATION ${CMAKE_INSTALL_DIR}
#							  PATH_VARS INCLUDE_INSTALL_DIR #SYSCONFIG_INSTALL_DIR
#							 )
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/carlConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/carlConfigVersion.cmake
#		DESTINATION ${CMAKE_INSTALL_DIR} )


	

# Install the export set for use with the install-tree
#install(EXPORT carlTargets DESTINATION ${CMAKE_INSTALL_DIR} COMPONENT dev)

# # # # # # # # # # # # # # # # # # # # # #
# G. CPack
# # # # # # # # # # # # # # # # # # # # # #


if (BINDINGS_PYTHON)
	add_subdirectory(bindings/pycarl)
endif()





# add_subdirectory(bindings)
