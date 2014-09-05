
# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties.
# B. Options.
# C. Find system tools and libraries.
# D. Write information files.
# E. Invoke subdirectories.
# F. Export Compile Information
# G. CPack
# # # # # # # # # # # # # # # # # # # # # #

cmake_minimum_required (VERSION 2.8 FATAL_ERROR)


# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties
# # # # # # # # # # # # # # # # # # # # # #
project(carl CXX)

set( PROJECT_FULLNAME "carl")
set( PROJECT_DESCRIPTION "Computer ARithmetic Library")
set( carl_MAJORVERSION 0)
set( carl_MINORVERSION 1)
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
option( DEVELOPER "Compile with extra warnings" ON )
option( STATICLIB_SWITCH "Compile a statically linked version of the library." OFF )
option( LOGGING "Enable logging within the carl library" ON )
option( LOGGING_CARL "Compile with the logging facility of the carl library" ON )
option( FORCE_SHIPPED_RESOURCES "Do not look in system for resources which are included" OFF )
option( COMPARE_WITH_GINAC "Compile with testing with the runtime and result comparisons of carl to ginac" OFF )
option( USE_COTIRE "Use cotire to generate and use precompiled headers" OFF )

# Offer the user the choice of overriding the installation directories
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for header files" )
set(LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
#set(SYSCONFIG_INSTALL_DIR etc/carl/  CACHE PATH "Instalation for system configuration files)
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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_USE_OWN_TR1_TUPLE=1")

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
	set(CMAKE_CXX_RELEASE_FLAGS "${CMAKE_CXX_RELEASE_FLAGS} -O3")
	set(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} -O1")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
		message(FATAL_ERROR " g++ version must be at least 4.8!")
	endif()
	message("-- Using g++")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wunknown-pragmas")
	set(CMAKE_CXX_RELEASE_FLAGS "${CMAKE_CXX_RELEASE_FLAGS} -O3")
	set(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} -O1")
else()
	message("-- Possibly unsupported compiler")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()
if(DEVELOPER)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wno-deprecated-declarations")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
	#set(CMAKE_BUILD_TYPE "DEBUG")
endif()

message("-- CXX Flags: ${CMAKE_CXX_FLAGS}")
message("-- CXX Debug Flags: ${CMAKE_CXX_DEBUG_FLAGS}")
message("-- CXX Release Flags: ${CMAKE_CXX_RELEASE_FLAGS}")
message("-- Build type: ${CMAKE_BUILD_TYPE}")

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

# We start by using CLN as numeric library. 
include( FindCLN )
# Default numeric library is GMP. 
include( FindGMP )
include( FindGMPXX )

set( libraries ${CLN_LIBRARY} ${GMPXX_LIBRARIES} ${GMP_LIBRARY} pthread )
set(include_dirs ${include_dirs} ${CLN_INCLUDE_DIR} ${GMPXX_INCLUDE_DIR} )

# # # # # # # # # # # # # # # # # # # # # #
# C.2. optional libraries
# # # # # # # # # # # # # # # # # # # # # #

# We use boost intervals for the interval arithmetic

#OPTION( BOOST_LOCAL "Use resources version of boost" OFF)
## If Boost was found, the Boost include directory will be saved in the variable
## Boost_INCLUDE_DIR.
#if(BOOST)
#SET(BOOST_NO_BOOST_CMAKE ON)
#SET(BOOST_NO_SYSTEM_PATHS ON)
#message(ERROR "No local version of boost exists")
##SET(BOOST_INCLUDEDIR "${PROJECT_SOURCE_DIR}/../resources/boost_1_55_0")
##SET(BOOST_LIBRARYDIR "${PROJECT_SOURCE_DIR}/../resources/boost_1_55_0")
#endif()
#FIND_PACKAGE(Boost COMPONENTS REQUIRED)

OPTION( OBTAIN_BOOST "Downloads and installs Boost in the resources folder." OFF )
if( NOT OBTAIN_BOOST )
find_package(Boost REQUIRED)
set( libraries ${libraries} ${Boost_LIBRARIES} )
set( include_dirs ${include_dirs} ${Boost_INCLUDE_DIR} )
else()
set( BOOST_REQUESTED_LIBRARIES interval)
endif()

OPTION( USE_MPFR_FLOAT "Use the mpfr implementation of floating point numbers." OFF )
IF(USE_MPFR_FLOAT)
	find_package(MPFR REQUIRED)
	set(include_dirs ${include_dirs} ${MPFR_INCLUDE_DIRS})
	set(libraries ${libraries} ${MPFR_LIBRARY})
endif()

set(libraries  ${libraries} )
set(include_dirs ${include_dirs} ${Boost_INCLUDE_DIR} )

# # # # # # # # # # # # # # # # # # # # # #
# C.3. Development and debug libraries.
# # # # # # # # # # # # # # # # # # # # # #

# if doxygen is installed, create the make doc target
include(FindDoxygen)
if(DOXYGEN)
	add_subdirectory(doxygen-conf)
else()
	message(WARNING, "Doxygen not found")
endif()

# we only search for this libraries in the system, if we do not force using the shipped resources.
if(NOT FORCE_SHIPPED_RESOURCES)
	# find googletest
	find_package(GTest)
	# find eigen
	find_package(Eigen3)
endif()
add_subdirectory(resources EXCLUDE_FROM_ALL)

enable_testing()
set(libraries ${libraries} ${LOG4CPLUS_LIBRARY} pthread)
#set(include_dirs ${include_dirs} ${LOG4CPLUS_INCLUDE_DIR} )

set(libraries ${libraries} ${EIGEN3_LIBRARY} pthread)
set(include_dirs ${include_dirs} ${EIGEN3_INCLUDE_DIR} )


if(COMPARE_WITH_GINAC)
	find_package( GiNaC 1.5.8 REQUIRED )
	set(libraries ${libraries} ${GINAC_LIBRARY} )
endif()

#dl must be linked LAST as this is required for the stacktrace in util/platform.h
set(libraries ${libraries} dl)

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
export(TARGETS lib_carl  FILE "${PROJECT_BINARY_DIR}/carlTargets.cmake")
 
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
