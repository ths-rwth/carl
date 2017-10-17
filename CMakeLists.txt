
# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties.
# B. Options.
# C. Find system tools and libraries.
# D. Write information files.
# E. Invoke subdirectories.
# F. Export Compile Information
# G. CPack
# # # # # # # # # # # # # # # # # # # # # #

cmake_minimum_required (VERSION 3.2 FATAL_ERROR)

# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties
# # # # # # # # # # # # # # # # # # # # # #
project(carl CXX)

# path to find own modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
# Include own macros.
include( carlmacros )

set_version(17 10)

set( PROJECT_FULLNAME "carl")
set( PROJECT_DESCRIPTION "Computer ARithmetic Library")
set( carl_NAME "CArL" )
set( carl_DESCRIPTION ${PROJECT_DESCRIPTION} )

message(STATUS "Version: ${PROJECT_FULLNAME} ${PROJECT_VERSION}")

# # # # # # # # # # # # # # # # # # # # # #
# B. Options
# # # # # # # # # # # # # # # # # # # # # #

# options
option( DEVELOPER "Compile with extra warnings" OFF )
option( ALLWARNINGS "Compile with even more warnings" OFF )
option( LOGGING "Enable logging within the carl library" OFF )
export_option(LOGGING)
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

set(CLANG_SANITIZER "none" CACHE STRING "Compile with the respective sanitizer")
set_property(CACHE CLANG_SANITIZER PROPERTY STRINGS none address memory thread)

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

include(clang-tidy)
include(coverage)

add_subdirectory(src)

include(export)

if (BINDINGS_PYTHON)
	add_subdirectory(bindings/pycarl)
endif()

include(resources/addons/addons.cmake)
include(packaging)
