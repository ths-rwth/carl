
# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties.
# B. Options.
# C. Find system tools and libraries.
# D. Write information files.
# E. Invoke subdirectories.
# F. CPack
# # # # # # # # # # # # # # # # # # # # # #

cmake_minimum_required (VERSION 2.8)

# # # # # # # # # # # # # # # # # # # # # #
# A. Project properties
# # # # # # # # # # # # # # # # # # # # # #
project(arithlib CXX)

set( PROJECT_FULLNAME "Arithmetic Lib")
set( PROJECT_DESCRIPTION "Something fancy")
set( arithlib_MAJORVERSION 0)
set( arithlib_MINORVERSION 1)
set( arithlib_MAINTENANCEVERSION 0)
set( arithlib_VERSION "${arithlib_MAJORVERSION}.${arithlib_MINORVERSION}.${arithlib_MAINTENANCEVERSION}")
set( PROJECT_VERSION "${arithlib_VERSION}")
set( smtrat_NAME "SMT-RAT" )
set( smtrat_DESCRIPTION ${PROJECT_DESCRIPTION} )


# # # # # # # # # # # # # # # # # # # # # #
# B. Options
# # # # # # # # # # # # # # # # # # # # # #

# options
option( STATICLIB_SWITCH "Compile a statically linked version of the library." OFF )

option( ARITHMETIC_LOGGING "Compile with the logging facility of the arithmetic library" ON )

# path to put in the executables
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

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
# include( FindCLN )
# find gmp (which is necessary for CLN, at least when linking statically)
# include( FindGMP )

set( libraries ${GINACRA_LIBRARY} ${GINAC_LIBRARY} ${CLN_LIBRARY} ${GMP_LIBRARY} pthread )

# # # # # # # # # # # # # # # # # # # # # #
# C.2. optional libraries
# # # # # # # # # # # # # # # # # # # # # #


# # # # # # # # # # # # # # # # # # # # # #
# C.3. Development and debug libraries.
# # # # # # # # # # # # # # # # # # # # # #

# CPP Unit is used for testing.
find_package(CPPUnit)

# if doxygen is installed, create the make doc target
include(FindDoxygen)
if(DOXYGEN)
  add_subdirectory(doxygen-conf)
else()
  message(WARNING, "Doxygen not found")
endif()

# find log4cplus


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