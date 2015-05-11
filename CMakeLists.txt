
# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties.
# B. Options.
# C. Find system tools and libraries.
# D. Write information files.
# E. Invoke subdirectories.
# F. Export Compile Information
# G. CPack
# # # # # # # # # # # # # # # # # # # # # #

if (WIN32)
	cmake_minimum_required (VERSION 3.1 FATAL_ERROR)
else()
	cmake_minimum_required (VERSION 2.8 FATAL_ERROR)
endif()

# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties
# # # # # # # # # # # # # # # # # # # # # #
project(carl CXX)

set( PROJECT_FULLNAME "carl")
set( PROJECT_DESCRIPTION "Computer ARithmetic Library")
set( carl_MAJORVERSION 1)
set( carl_MINORVERSION 0)
set( carl_MAINTENANCEVERSION 0)
set( carl_VERSION "${carl_MAJORVERSION}.${carl_MINORVERSION}.${carl_MAINTENANCEVERSION}")
set( PROJECT_VERSION "${carl_VERSION}")
set( carl_NAME "CArL" )
set( carl_DESCRIPTION ${PROJECT_DESCRIPTION} )


# path to find own modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
# for cmake 2.8.7 
include(FindPackageHandleStandardArgs) 

# # # # # # # # # # # # # # # # # # # # # #
# B. Options
# # # # # # # # # # # # # # # # # # # # # #

# options
option( DEVELOPER "Compile with extra warnings" OFF )
option( LOGGING "Enable logging within the carl library" OFF )
option( LOGGING_CARL "Compile with the logging facility of the carl library" OFF )
option( LOGGING_DISABLE_INEFFICIENT "Disable log messages about inefficient methods" OFF )
option( FORCE_SHIPPED_RESOURCES "Do not look in system for resources which are included" OFF )
option( USE_GINAC "Compile with testing with the runtime and result comparisons of carl to ginac" OFF )
option( COMPARE_WITH_Z3 "Compile benchmarks that compare to z3" OFF )
option( USE_Z3_NUMBERS "Make z3 rationals available in carl" OFF )
option( USE_CLN_NUMBERS "Make cln rationals and integers available in carl" OFF )
option( USE_COTIRE "Use cotire to generate and use precompiled headers" OFF )
option( BUILD_STATIC "Build the static library as well" OFF )
option( THREAD_SAFE "Use mutexing to assure thread safety" OFF )
option( PRUNE_MONOMIAL_POOL "Prune monomial pool" ON )

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RELEASE")
endif()

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

if (NOT WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_USE_OWN_TR1_TUPLE=1")
endif()

# compiler flags.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.3)
		message(FATAL_ERROR " clang version must be at least 3.3!")
	endif()
	if (APPLE)
		message("-- Using clang on apple")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
	else()
		message("-- Using clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	endif()
	set(CMAKE_CXX_RELEASE_FLAGS "${CMAKE_CXX_RELEASE_FLAGS} -O3 -fomit-frame-pointer -msse -msse2 -funroll-loops")
	set(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} -O1")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
		message(FATAL_ERROR " g++ version must be at least 4.8!")
	endif()
	message("-- Using g++")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wunknown-pragmas")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL 4.9 OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.9)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmax-errors=5 -fdiagnostics-color=auto")
	endif()
	set(CMAKE_CXX_RELEASE_FLAGS "${CMAKE_CXX_RELEASE_FLAGS} -O3")
	set(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} -O1")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	message("-- Using Visual Studio C++")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	set(CMAKE_CXX_RELEASE_FLAGS "${CMAKE_CXX_RELEASE_FLAGS} /O2")
	set(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} /MTd")
else()
	message("-- Possibly unsupported compiler")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()
if(DEVELOPER)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wno-deprecated-declarations")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -ftemplate-backtrace-limit=0")
	set(CMAKE_BUILD_TYPE "DEBUG")
endif()

message("-- CXX Flags: ${CMAKE_CXX_FLAGS}")
message("-- CXX Debug Flags: ${CMAKE_CXX_DEBUG_FLAGS}")
message("-- CXX Release Flags: ${CMAKE_CXX_RELEASE_FLAGS}")
message("-- Build type: ${CMAKE_BUILD_TYPE}")

set(DYNAMIC_EXT "so")
set(STATIC_EXT "a")
if (APPLE)
	set(DYNAMIC_EXT "dylib")
	set(STATIC_EXT "a")
elseif (WIN32)
	set(DYNAMIC_EXT "dll")
	set(STATIC_EXT "lib")
endif()
message("-- Assuming extension for shared libraries: .${DYNAMIC_EXT}")
message("-- Assuming extension for static libraries: .${STATIC_EXT}")

if(BUILD_STATIC)
	message("-- Building static: yes")
else()
	message("-- Building static: no")
endif()


if(USE_COTIRE)
	# We use cotire, simply include it
	message("-- using cotire")
	include(cotire)
else()
	# We do not use cotire, create dummy function.
	message("-- not using cotire")
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

# Boost
set(Boost_COMPONENTS )
set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost REQUIRED COMPONENTS ${Boost_COMPONENTS})
set(Boost_DYNAMIC ${Boost_LIBRARIES})
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS ${Boost_COMPONENTS})
set(Boost_STATIC ${Boost_LIBRARIES})


if(USE_GINAC)
    set(USE_CLN_NUMBERS ON)
endif()

# CLN
IF(USE_CLN_NUMBERS)
    find_package(CLN REQUIRED)
    find_library(CLN_DYNAMIC NAMES libcln.${DYNAMIC_EXT})
    find_library(CLN_STATIC NAMES libcln.${STATIC_EXT})
endif()

# GMP
find_package(GMP REQUIRED)
find_library(GMP_DYNAMIC NAMES libgmp.${DYNAMIC_EXT})
find_library(GMP_STATIC NAMES libgmp.${STATIC_EXT})
find_package(GMPXX REQUIRED)
find_library(GMPXX_DYNAMIC NAMES libgmpxx.${DYNAMIC_EXT})
find_library(GMPXX_STATIC NAMES libgmpxx.${STATIC_EXT})

include_directories(SYSTEM ${include_dirs} ${Boost_INCLUDE_DIRS})
IF(USE_CLN_NUMBERS)
    include_directories(SYSTEM ${SYSTEM} ${CLN_INCLUDE_DIR})
endif()
include_directories(SYSTEM ${SYSTEM} ${EIGEN3_INCLUDE_DIR} ${GMPXX_INCLUDE_DIR} ${GMP_INCLUDE_DIR} )

set(libs_dynamic ${libs_dynamic} ${Boost_DYNAMIC})
IF(USE_CLN_NUMBERS)
    set(libs_dynamic ${libs_dynamic} ${CLN_DYNAMIC})
endif()
set(libs_dynamic ${libs_dynamic} ${GMPXX_DYNAMIC} ${GMP_DYNAMIC})

set(libs_static ${libs_static} ${Boost_STATIC})
IF(USE_CLN_NUMBERS)
    set(libs_static ${libs_static} ${CLN_STATIC})
endif()
set(libs_static ${libs_static} ${GMPXX_STATIC} ${GMP_STATIC})

# # # # # # # # # # # # # # # # # # # # # #
# C.2. optional libraries
# # # # # # # # # # # # # # # # # # # # # #

OPTION( USE_MPFR_FLOAT "Use the mpfr implementation of floating point numbers." OFF )
IF(USE_MPFR_FLOAT)
	find_package(MPFR REQUIRED)
	find_library(MPFR_DYNAMIC NAMES libmpfr.${DYNAMIC_EXT})
	find_library(MPFR_STATIC NAMES libmpfr.${STATIC_EXT})
	
	include_directories(SYSTEM ${include_dirs} ${MPFR_INCLUDE_DIRS})
	set(libs_dynamic ${libs_dynamic} ${MPFR_DYNAMIC})
	set(libs_static ${libs_static} ${MPFR_STATIC})
endif()

if(COMPARE_WITH_Z3)
    find_package(Z3)
    include_directories(${Z3_INCLUDE_DIRS})
endif()

# # # # # # # # # # # # # # # # # # # # # #
# C.3. Development and debug libraries.
# # # # # # # # # # # # # # # # # # # # # #

# if doxygen is installed, create the make doc target
find_package(Doxygen)
if(DOXYGEN)
	add_subdirectory(doxygen-conf)
else()
	message(WARNING, "Doxygen not found")
endif()

# we only search for this libraries in the system, if we do not force using the shipped resources.
if(NOT FORCE_SHIPPED_RESOURCES)
	# find googletest
	find_package(GTest)
	find_package(Eigen3)
endif()
add_subdirectory(resources EXCLUDE_FROM_ALL)
include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})


enable_testing()

if(USE_GINAC)
	find_package( GiNaC 1.5.8 REQUIRED )
	find_library(GiNaC_DYNAMIC NAMES libginac.${DYNAMIC_EXT})
	find_library(GiNaC_STATIC NAMES libginac.${STATIC_EXT})
	include_directories(SYSTEM ${include_dirs} ${GINAC_INCLUDE_DIRS})
	set(libs_dynamic ${libs_dynamic} ${GiNaC_DYNAMIC})
	set(libs_static ${libs_static} ${GiNaC_STATIC})
	set(USE_CLN_NUMBERS ON)
endif()

#dl must be linked LAST as this is required for the stacktrace in util/platform.h
if (NOT WIN32)
	set(libs_dynamic ${libs_dynamic} pthread dl)
	set(libs_static ${libs_static} pthread dl)
endif()

# # # # # # # # # # # # # # # # # # # # # #
# D. Writen information files
# # # # # # # # # # # # # # # # # # # # # #

# Getting git info (To be added later)
#include(GetGitRevisionDescription)
#get_git_head_revision(GIT_REFSPEC GIT_SHA1)

# # # # # # # # # # # # # # # # # # # # # #
# E. Invoke subdirectories.
# # # # # # # # # # # # # # # # # # # # # #

add_subdirectory(src)

# # # # # # # # # # # # # # # # # # # # # #
# F. Export Compile Information
# # # # # # # # # # # # # # # # # # # # # #

# Add all targets to the build-tree export set
export(TARGETS ${CARL_TARGETS} FILE "${PROJECT_BINARY_DIR}/carlTargets.cmake")
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE carl)
 
include(CMakePackageConfigHelpers)
# Create the carlConfig.cmake and carlConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${CMAKE_INSTALL_DIR}" "${INCLUDE_INSTALL_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src")
configure_package_config_file(cmake/carlConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/carlConfig.cmake
							  INSTALL_DESTINATION ${CMAKE_INSTALL_DIR}
							  PATH_VARS INCLUDE_INSTALL_DIR #SYSCONFIG_INSTALL_DIR
							 )
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${carl_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_package_config_file(cmake/carlConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/carlConfig.cmake
							  INSTALL_DESTINATION ${CMAKE_INSTALL_DIR}
							  PATH_VARS INCLUDE_INSTALL_DIR #SYSCONFIG_INSTALL_DIR
							 )
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/carlConfigVersion.cmake
								 VERSION 0.1.0
								 COMPATIBILITY SameMajorVersion )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/carlConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/carlConfigVersion.cmake
		DESTINATION ${CMAKE_INSTALL_DIR} )
 
# Install the export set for use with the install-tree
install(EXPORT carlTargets DESTINATION ${CMAKE_INSTALL_DIR} COMPONENT dev)

# # # # # # # # # # # # # # # # # # # # # #
# G. CPack
# # # # # # # # # # # # # # # # # # # # # #

# todo.
