
# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties.
# B. Options.
# C. Find system tools and libraries.
# D. Write information files.
# E. Invoke subdirectories.
# F. CPack
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
set( carl_VERSION "${arithlib_MAJORVERSION}.${arithlib_MINORVERSION}.${arithlib_MAINTENANCEVERSION}")
set( PROJECT_VERSION "${arithlib_VERSION}")
set( carl_NAME "CArL" )
set( carl_DESCRIPTION ${PROJECT_DESCRIPTION} )


# path to find own modules
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# # # # # # # # # # # # # # # # # # # # # #
# B. Options
# # # # # # # # # # # # # # # # # # # # # #

# options
option( STATICLIB_SWITCH "Compile a statically linked version of the library." OFF )

option( CARL_LOGGING "Compile with the logging facility of the carl library" ON )
option( FORCE_SHIPPED_RESOURCES "Do not look in system for resources which are included" OFF )

# path to put in the executables
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
# compiler flags.
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -std=c++0x")

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

set( libraries ${GMP_LIBRARY}  ${GMPXX_LIBRARIES} ${CLN_LIBRARY} pthread )
set(include_dirs ${include_dirs} ${CLN_INCLUDE_DIR} ${GMPXX_INCLUDE_DIR} )

# # # # # # # # # # # # # # # # # # # # # #
# C.2. optional libraries
# # # # # # # # # # # # # # # # # # # # # #

# We use boost intervals for the interval arithmetic

OPTION( BOOST_LOCAL "Use resources version of boost" OFF)
# If Boost was found, the Boost include directory will be saved in the variable
# Boost_INCLUDE_DIR.
if(BOOST_LOCAL)
SET(BOOST_NO_BOOST_CMAKE ON)

SET(BOOST_NO_SYSTEM_PATHS ON)
message(ERROR "No local version of boost exists")
#SET(BOOST_INCLUDEDIR "${PROJECT_SOURCE_DIR}/../resources/boost_1_53_0")
#SET(BOOST_LIBRARYDIR "${PROJECT_SOURCE_DIR}/../resources/boost_1_53_0")
endif()
FIND_PACKAGE(Boost COMPONENTS REQUIRED)

set( libraries  ${libraries} )
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
	# find log4cplus
	find_package(Log4cplus)
	# find googletest
	find_package(GTest)
endif()
add_subdirectory(resources EXCLUDE_FROM_ALL)

enable_testing()
set(libraries ${libraries} ${LOG4CPLUS_LIBRARY} pthread)
set(include_dirs ${include_dirs} ${LOG4CPLUS_INCLUDE_DIR} )


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
# F. CPack
# # # # # # # # # # # # # # # # # # # # # #

# TODO.