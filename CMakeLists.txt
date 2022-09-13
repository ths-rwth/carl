cmake_minimum_required (VERSION 3.7 FATAL_ERROR)

#set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(carl CXX)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not allowed.")
endif()

# path to find own modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Include own macros.
include(carlmacros)

set_version(22 09)

set(PROJECT_FULLNAME "carl")
set(PROJECT_DESCRIPTION "Computer ARithmetic Library")
set(carl_NAME "CArL" )
set(carl_DESCRIPTION ${PROJECT_DESCRIPTION} )

message(STATUS "Version: ${PROJECT_FULLNAME} ${PROJECT_VERSION_FULL}")

# # # # # # # # # # # # # # # # # # # # # #
# CMake Options
# # # # # # # # # # # # # # # # # # # # # #

option( DEVELOPER "Compile with extra warnings" OFF )
option( ALLWARNINGS "Compile with even more warnings" OFF )
option( LOGGING "Enable logging within the carl library" OFF )
export_option(LOGGING)
option( LOGGING_DISABLE_INEFFICIENT "Disable log messages about inefficient methods" OFF )
option( CARL_DEVOPTION_Checkpoints "Enable checkpoints within the carl library" OFF )
option( CARL_DEVOPTION_Statistics "Enable statistics and timing within the carl library" OFF )
export_option(CARL_DEVOPTION_Statistics)
option( FORCE_SHIPPED_RESOURCES "Do not look in system for resources which are included" OFF )
export_option(FORCE_SHIPPED_RESOURCES)
option( FORCE_SHIPPED_GMP "Do not look in system for lib gmp" OFF )
export_option(FORCE_SHIPPED_GMP)
option(USE_LIBPOLY "Use LibPoly" ON)
export_option(USE_LIBPOLY)
option( USE_GINAC "Compile with testing with the runtime and result comparisons of carl to ginac" OFF )
export_option(USE_GINAC)
option( USE_CLN_NUMBERS "Make cln rationals and integers available in carl" OFF )
export_option(USE_CLN_NUMBERS)
option( USE_COCOA "Use CoCoALib" ON )
export_option(USE_COCOA)
option( USE_BLISS "Use bliss" ON )
export_option(USE_BLISS)
OPTION( USE_MPFR_FLOAT "Use the mpfr implementation of floating point numbers." OFF )
export_option(USE_MPFR_FLOAT)
option( USE_COTIRE "Use cotire to generate and use precompiled headers" OFF )
option( THREAD_SAFE "Use mutexing to assure thread safety" OFF )
export_option(THREAD_SAFE)



set(CLANG_SANITIZER "none" CACHE STRING "Compile with the respective sanitizer")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RELEASE" CACHE STRING "Build type." FORCE)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "DEBUG" "RELEASE")

set(CARL_LIBRARIES_DIR "${PROJECT_BINARY_DIR}/lib")

# Offer the user the choice of overriding the installation directories
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for header files" )
set(LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
#set(SYSCONFIG_INSTALL_DIR etc/carl/  CACHE PATH "Installation for system configuration files)
set(BIN_INSTALL_DIR lib/ CACHE PATH "Installation directory for executables")


if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/carl)
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

include(cotire)

# RPATH settings
# don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already (but only when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# the RPATH to be used when installing
SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# don't add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# # # # # # # # # # # # # # # # # # # # # #
# Required libraries
# # # # # # # # # # # # # # # # # # # # # #
include(resources/resources.cmake)

enable_testing()

#dl must be linked LAST as this is required for the stacktrace in carl-common/meta/platform.h
if (NOT WIN32)
	list(APPEND carl_LIBRARIES_DYNAMIC pthread dl)
endif()

# Configure static analysis
include(clang-tidy)
include(clang-time-trace)
include(coverage)

# Configure documentation
add_subdirectory(doc)

# Include actual library
add_subdirectory(src)

# Configure export
include(export)

# Configure bindings
if (BINDINGS_PYTHON)
	add_subdirectory(bindings/pycarl)
endif()

# Configure addons
include(resources/addons/addons.cmake)

# Configure packaging and CI
include(packaging)
include(prepare-ci)
