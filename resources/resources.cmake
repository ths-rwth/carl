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
endif()

###############
##### Load resources
#####
##### Note that the resources may have dependencies among each other.
###############

##### GMP / GMPXX
if((NOT FORCE_SHIPPED_RESOURCES) AND (NOT FORCE_SHIPPED_GMP))
	load_library(carl GMP 5.1)
	load_library(carl GMPXX 5.1)
endif()
if(NOT GMP_FOUND)
	set(GMP_VERSION "6.1.0")
	include(resources/gmp.cmake)
endif()
print_resource_info("GMP / GMPXX" GMP_SHARED ${GMP_VERSION})

##### Boost
set(BOOST_COMPONENTS "filesystem;system;program_options;regex;unit_test_framework")
set(Boost_USE_DEBUG_RUNTIME "OFF")
if(NOT FORCE_SHIPPED_RESOURCES)
	load_library(carl Boost 1.58 COMPONENTS ${BOOST_COMPONENTS})
endif()
if(NOT Boost_FOUND)
	set(Boost_VERSION "1.64.0")
	set(Boost_ZIPHASH "36093e4018aecd5b0e31e80457ac5fc1")
	include(resources/boost.cmake)
	unset(Boost_ZIPHASH)
endif()
print_resource_info("Boost" Boost_SHARED ${Boost_VERSION})

if(Boost_VERSION VERSION_GREATER_EQUAL "106500")
	set(BOOST_HAS_STACKTRACE "1")
endif()

##### Eigen3
if(NOT FORCE_SHIPPED_RESOURCES)
	load_library(carl EIGEN3 3.3)
endif()
if(NOT EIGEN3_FOUND)
	set(EIGEN3_VERSION "3.3.4")
	set(EIGEN3_ZIPHASH "e337acc279874bc6a56da4d973a723fb")
	include(resources/eigen3.cmake)
	unset(EIGEN3_ZIPHASH)
endif()
print_resource_info("Eigen3" EIGEN3 ${EIGEN3_VERSION})

##### bliss
if(USE_BLISS)
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl BLISS 0.73)
	endif()
	if(NOT BLISS_FOUND)
		set(BLISS_VERSION "0.73")
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
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl CoCoA 0.99564)
	endif()
	if(NOT COCOA_FOUND)
		set(COCOA_VERSION "0.99600")
		set(COCOA_TGZHASH "e32a169ea252640649e2742856330110")
		include(resources/cocoa.cmake)
		unset(COCOA_TGZHASH)
	endif()
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
if(NOT GTEST_FOUND)
	set(GTEST_VERSION "1.8.0")
	set(GTEST_ZIPHASH "adfafc8512ab65fd3cf7955ef0100ff5")
	include(resources/gtest.cmake)
	unset(GTEST_ZIPHASH)
endif()
print_resource_info("GTest" GTESTMAIN_STATIC ${GTEST_VERSION})

##### MPFR
IF(USE_MPFR_FLOAT)
	load_library(carl MPFR 0.0 REQUIRED)
endif()

##### Z3
if(COMPARE_WITH_Z3)
    load_library(carl Z3 4.0)
endif()

##### Doxygen
find_package(Doxygen 1.8.9 QUIET)
if(DOXYGEN_FOUND AND ${CMAKE_VERSION} VERSION_LESS "3.9.0")
	add_executable(Doxygen::doxygen IMPORTED GLOBAL)
	set_target_properties(Doxygen::doxygen PROPERTIES IMPORTED_LOCATION "${DOXYGEN_EXECUTABLE}")
endif()
if(NOT DOXYGEN_FOUND AND BUILD_DOXYGEN)
	set(DOXYGEN_VERSION "1.8.14")
	include(resources/doxygen.cmake)
	add_custom_target(doxygen-build DEPENDS Doxygen::doxygen Doxygen-EP)
else()
	add_custom_target(doxygen-build DEPENDS Doxygen::doxygen)
endif()
print_resource_info("Doxygen" Doxygen::doxygen "${DOXYGEN_VERSION}")
