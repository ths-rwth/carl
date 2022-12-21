include(ExternalProject)
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/resources)

add_custom_target(resources)

###############
##### Generic resource configuration
###############
if("${CMAKE_GENERATOR}" MATCHES "Make")
	set(CMAKE_MAKE_PROGRAM "$(MAKE)")
endif()

# Make sure that libraries from /usr/lib et al are found before OSX frameworks
set(CMAKE_FIND_FRAMEWORK "LAST")

function(print_resource_info name target version)
	if(TARGET ${target})
		get_target_property(TYPE ${target} TYPE)
		if(TYPE STREQUAL "EXECUTABLE")
			get_target_property(PATH1 ${target} IMPORTED_LOCATION)
		elseif(TYPE STREQUAL "SHARED_LIBRARY")
			get_target_property(PATH1 ${target} INTERFACE_INCLUDE_DIRECTORIES)
			get_target_property(PATH2 ${target} IMPORTED_LOCATION)
		elseif(TYPE STREQUAL "STATIC_LIBRARY")
			get_target_property(PATH1 ${target} INTERFACE_INCLUDE_DIRECTORIES)
			get_target_property(PATH2 ${target} IMPORTED_LOCATION)
		elseif(TYPE STREQUAL "INTERFACE_LIBRARY")
			get_target_property(PATH1 ${target} INTERFACE_INCLUDE_DIRECTORIES)
			get_target_property(PATH2 ${target} INTERFACE_LINK_LIBRARIES)
		endif()
		if(PATH1 AND PATH2)
			message(STATUS "${name} ${version} was found at ${PATH1} and ${PATH2}")
		else()
			message(STATUS "${name} ${version} was found at ${PATH1}")
		endif()
	else()
		message(STATUS "${name} was not found.")
	endif()
endfunction(print_resource_info)

###############
##### Process resource dependencies
###############
if(USE_GINAC)
	set(USE_CLN_NUMBERS ON)
elseif(USE_CLN_NUMBERS)
	set(USE_GINAC ON)
endif()

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
cmake_policy(SET CMP0135 NEW)
endif()

###############
##### Load resources
#####
##### Note that the resources may have dependencies among each other.
###############

##### GMP / GMPXX
if((NOT FORCE_SHIPPED_RESOURCES) AND (NOT FORCE_SHIPPED_GMP))
	load_library(carl GMP 6.1)
	load_library(carl GMPXX 6.1)
endif()
if(NOT GMP_FOUND)
	set(GMP_VERSION "6.1.0")
	include(resources/gmp.cmake)
endif()
print_resource_info("GMP / GMPXX" GMP_SHARED ${GMP_VERSION})

##### Boost
set(BOOST_COMPONENTS "system;program_options;unit_test_framework;timer;chrono;serialization")
set(Boost_USE_DEBUG_RUNTIME "OFF")
if(NOT FORCE_SHIPPED_RESOURCES)
	find_package(Boost 1.65.1 COMPONENTS ${BOOST_COMPONENTS})
endif()
if(NOT Boost_FOUND)
	set(Boost_VERSION "1.70.0")
	set(Boost_ZIPHASH "48f379b2e90dd1084429aae87d6bdbde9670139fa7569ee856c8c86dd366039d")
	include(resources/boost.cmake)
	unset(Boost_ZIPHASH)
endif()
print_resource_info("Boost" Boost_SHARED ${Boost_VERSION})

##### Eigen3
if(NOT FORCE_SHIPPED_RESOURCES)
	load_library(carl EIGEN3 3.3)
endif()
if(NOT EIGEN3_FOUND)
	set(EIGEN3_VERSION "3.4.0")
	include(resources/eigen3.cmake)
endif()
print_resource_info("Eigen3" EIGEN3 ${EIGEN3_VERSION})

##### bliss
if(USE_BLISS)
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl BLISS 0.73)
	endif()
	if(NOT BLISS_FOUND)
		set(BLISS_VERSION "0.74")
		include(resources/bliss.cmake)
	endif()
	print_resource_info("Bliss" BLISS_SHARED ${BLISS_VERSION})
else()
	message(STATUS "Bliss is disabled")
endif()

##### CLN
if(USE_CLN_NUMBERS)
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl CLN 1.3)
	endif()
	if(NOT CLN_FOUND)
		set(CLN_VERSION "1.3.4")
		include(resources/cln.cmake)
	endif()
	set_target_properties(CLN_STATIC PROPERTIES LINK_INTERFACE_LIBRARIES "GMP_STATIC")
	print_resource_info("CLN" CLN_SHARED ${CLN_VERSION})
else()
	message(STATUS "CLN is disabled")
endif()


##### CoCoALib
if(USE_COCOA)
	set(COCOA_VERSION "0.99800")
	set(COCOA_TGZHASH "f8bb227e2e1729e171cf7ac2008af71df25914607712c35db7bcb5a044a928c6")
	load_library(carl CoCoA ${COCOA_VERSION} REQUIRED)
	unset(COCOA_TGZHASH)
	print_resource_info("CoCoA" COCOA_SHARED ${COCOA_VERSION})
else()
	message(STATUS "CoCoA is disabled")
endif()

##### GiNaC
if(USE_GINAC)
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl GINAC 1.6.2)
	endif()
	if(NOT GINAC_FOUND)
		set(GINAC_VERSION "1.7.2")
		include(resources/ginac.cmake)
	endif()
	print_resource_info("GiNaC" GINAC_SHARED ${GINAC_VERSION})
else()
	message(STATUS "GiNaC is disabled")
endif()


##### GTest
set(GTEST_VERSION "1.12.1")
set(GTEST_ZIPHASH "973e464e8936d4b79bb24f27b058aaef4150b06e")
load_library(carl GTest ${GTEST_VERSION} REQUIRED)
print_resource_info("GTest" GTESTCORE_STATIC ${GTEST_VERSION})

include(resources/google-benchmark.cmake)

##### MPFR
IF(USE_MPFR_FLOAT)
	load_library(carl MPFR 0.0 REQUIRED)
endif()

##### LIBPOLY
IF(USE_LIBPOLY)
	set(LIBPOLY_VERSION "0.1.11")
	load_library(carl Poly ${LIBPOLY_VERSION} REQUIRED)
	print_resource_info("LibPoly" LIBPOLYXX_STATIC ${LIBPOLY_VERSION})
endif()


##### Doxygen
find_package(Doxygen 1.8.14 QUIET)
if(DOXYGEN_FOUND AND ${CMAKE_VERSION} VERSION_LESS "3.9.0")
	add_executable(Doxygen::doxygen IMPORTED GLOBAL)
	set_target_properties(Doxygen::doxygen PROPERTIES IMPORTED_LOCATION "${DOXYGEN_EXECUTABLE}")
endif()
print_resource_info("Doxygen" Doxygen::doxygen "${DOXYGEN_VERSION}")