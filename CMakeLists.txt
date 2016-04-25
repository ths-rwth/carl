
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
	cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
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
option( ALLWARNINGS "Compile with even more warnings" OFF )
option( LOGGING "Enable logging within the carl library" OFF )
option( LOGGING_CARL "Compile with the logging facility of the carl library" OFF )
option( LOGGING_DISABLE_INEFFICIENT "Disable log messages about inefficient methods" OFF )
option( FORCE_SHIPPED_RESOURCES "Do not look in system for resources which are included" OFF )
option( USE_GINAC "Compile with testing with the runtime and result comparisons of carl to ginac" OFF )
option( COMPARE_WITH_Z3 "Compile benchmarks that compare to z3" OFF )
option( USE_Z3_NUMBERS "Make z3 rationals available in carl" OFF )
option( USE_CLN_NUMBERS "Make cln rationals and integers available in carl" OFF )
OPTION( USE_MPFR_FLOAT "Use the mpfr implementation of floating point numbers." OFF )
option( USE_COTIRE "Use cotire to generate and use precompiled headers" OFF )
option( BUILD_STATIC "Build the static library as well" OFF )
option( THREAD_SAFE "Use mutexing to assure thread safety" OFF )
option( PRUNE_MONOMIAL_POOL "Prune monomial pool" ON )
option( CLANG_TIDY "Enable usage of clang-tidy" OFF )

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RELEASE")
endif()

# Include own macros.
include( carlmacros )

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
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)
		message(FATAL_ERROR " clang version must be at least 3.5!")
	endif()
	if (APPLE)
		message(STATUS "Using clang on apple")
                set(CMAKE_MACOSX_RPATH 1)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -stdlib=libc++")
	else()
		message(STATUS "Using clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	endif()
	set(CMAKE_CXX_RELEASE_FLAGS "${CMAKE_CXX_RELEASE_FLAGS} -O3 -fomit-frame-pointer -msse -msse2 -funroll-loops")
	set(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} -O1")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
		message(FATAL_ERROR " g++ version must be at least 4.9!")
	endif()
	message(STATUS "Using g++")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wunknown-pragmas")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmax-errors=5 -fdiagnostics-color=auto")
	set(CMAKE_CXX_RELEASE_FLAGS "${CMAKE_CXX_RELEASE_FLAGS} -O3")
	set(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} -O1")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	message("-- Using Visual Studio C++")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	set(CMAKE_CXX_RELEASE_FLAGS "${CMAKE_CXX_RELEASE_FLAGS} /O2 /MT")
	set(CMAKE_CXX_DEBUG_FLAGS "${CMAKE_CXX_DEBUG_FLAGS} /MTd")
else()
	message(WARNING "Possibly unsupported compiler")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()
if(DEVELOPER)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wno-deprecated-declarations")
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -ftemplate-backtrace-limit=0")
	#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -print-search-dirs")

	if (ALLWARNINGS)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-old-style-cast")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reserved-id-macro")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-newline-eof")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-documentation")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch-enum")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-covered-switch-default")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-padded")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-float-equal")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedef")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-variable-declarations")
	endif()
	endif()

	set(CMAKE_BUILD_TYPE "DEBUG")
endif()

message(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "CXX Debug Flags: ${CMAKE_CXX_DEBUG_FLAGS}")
message(STATUS "CXX Release Flags: ${CMAKE_CXX_RELEASE_FLAGS}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(DYNAMIC_EXT ".so")
set(STATIC_EXT ".a")
if(APPLE)
	set(DYNAMIC_EXT ".dylib")
	set(STATIC_EXT ".a")
elseif (WIN32)
	set(DYNAMIC_EXT ".dll")
	set(STATIC_EXT ".lib")
endif()
message(STATUS "Assuming extension for shared libraries: ${DYNAMIC_EXT}")
message(STATUS "Assuming extension for static libraries: ${STATIC_EXT}")

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

# Boost
load_library(carl Boost 1.29 REQUIRED)

include_directories(SYSTEM ${include_dirs} ${Boost_INCLUDE_DIRS})
list(APPEND carl_LIBRARIES_DYNAMIC ${Boost_DYNAMIC})
list(APPEND carl_LIBRARIES_STATIC ${Boost_STATIC})


if(USE_GINAC)
	load_library(carl GiNaC 1.5.8 REQUIRED )
    set(USE_CLN_NUMBERS ON)
endif()

# CLN
IF(USE_CLN_NUMBERS)
	load_library(carl CLN 0.0 REQUIRED)
endif()

# GMP
load_library(carl GMPXX 0.0 REQUIRED)
load_library(carl GMP 0.0 REQUIRED)

# # # # # # # # # # # # # # # # # # # # # #
# C.2. optional libraries
# # # # # # # # # # # # # # # # # # # # # #

IF(USE_MPFR_FLOAT)
	load_library(carl MPFR 0.0 REQUIRED)
endif()

if(COMPARE_WITH_Z3)
    load_library(carl Z3 4.0)
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
	load_library(carl GTest 0.0)
	load_library(carl Eigen3 0.0)
endif()
add_subdirectory(resources EXCLUDE_FROM_ALL)

enable_testing()

message(STATUS "include: ${carl_INCLUDE_DIRS}")
message(STATUS "dynamic: ${carl_LIBRARIES_DYNAMIC}")
message(STATUS "static: ${carl_LIBRARIES_STATIC}")
include_directories(SYSTEM ${SYSTEM} ${carl_INCLUDE_DIRS})

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

if (CLANG_TIDY)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	set(CLANG_TIDY_CHECKS "*")
	set(CLANG_TIDY_CHECKS "${CLANG_TIDY_CHECKS},-llvm-header-guard")
	set(CLANG_TIDY_CHECKS "${CLANG_TIDY_CHECKS},-google-readability-namespace-comments,-llvm-namespace-comment")
	set(CLANG_TIDY_CHECKS "${CLANG_TIDY_CHECKS},-google-readability-braces-around-statements,-readability-braces-around-statements")
	set(CLANG_TIDY_CHECKS "${CLANG_TIDY_CHECKS},-readability-else-after-return")
	set(CLANG_TIDY_CHECKS "${CLANG_TIDY_CHECKS},-google-readability-todo")
	set(CLANG_TIDY_CHECKS "${CLANG_TIDY_CHECKS},-google-runtime-int")
	set(CLANG_TIDY_CHECKS "${CLANG_TIDY_CHECKS},-misc-macro-parentheses")
	set(CLANG_TIDY_CHECKS "${CLANG_TIDY_CHECKS},-clang-analyzer-alpha.core.CastToStruct")
	set(CLANG_TIDY_CHECKS "-checks='${CLANG_TIDY_CHECKS}'")
	message(STATUS "Enabled checks for clang-tidy: ${CLANG_TIDY_CHECKS}")
endif()

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
