include(ExternalProject)
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/resources)

add_custom_target(resources)

###############
##### Generic resource configuration
###############
if("${CMAKE_GENERATOR}" MATCHES "Make")
	set(CMAKE_MAKE_PROGRAM "$(MAKE)")
endif()

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
if(NOT FORCE_SHIPPED_RESOURCES)
	load_library(carl GMP 5.1)
	load_library(carl GMPXX 5.1)
endif()
if(GMP_FOUND)
	message(STATUS "Use system version of GMP/GMPXX ${GMP_VERSION}")
else()
	set(GMP_VERSION "6.1.0")
	include(resources/gmp.cmake)
	message(STATUS "Use shipped version of GMP/GMPXX ${GMP_VERSION}")
endif()

##### Boost
set(BOOST_COMPONENTS "system;program_options;regex")
set(Boost_USE_DEBUG_RUNTIME "OFF")
if(NOT FORCE_SHIPPED_RESOURCES)
	load_library(carl Boost 1.55 COMPONENTS ${BOOST_COMPONENTS})
endif()
if(Boost_FOUND)
	message(STATUS "Use system version of Boost ${Boost_VERSION}")
else()
	set(BOOST_VERSION "1.60.0")
	include(resources/boost.cmake)
	message(STATUS "Use shipped version of Boost ${BOOST_VERSION}")
endif()


##### CLN
if(USE_CLN_NUMBERS)
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl CLN 1.3)
	endif()
	if(CLN_FOUND)
		message(STATUS "Use system version of CLN ${CLN_VERSION}")
	else()
		set(CLN_VERSION "1.3.4")
		include(resources/cln.cmake)
		message(STATUS "Use shipped version of CLN ${CLN_VERSION}")
	endif()
	set_target_properties(CLN_STATIC PROPERTIES LINK_INTERFACE_LIBRARIES "GMP_STATIC")
endif()


##### CoCoALib
if(USE_COCOA)
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl CoCoA 0.99542)
	endif()
	if(COCOA_FOUND)
		message(STATUS "Use system version of CoCoA ${COCOA_VERSION}")
	else()
		# Depends on GMP
		set(COCOA_VERSION "0.99551")
		set(COCOA_TGZHASH "0e75ba96e627f955adbb17c037d5bcdf")
		include(resources/cocoa.cmake)
		unset(COCOA_TGZHASH)
		message(STATUS "Use shipped version of CoCoA ${COCOA_VERSION}")
	endif()
endif()


##### Eigen3
if(NOT FORCE_SHIPPED_RESOURCES)
	load_library(carl EIGEN3 3.2)
endif()
if(EIGEN3_FOUND)
	message(STATUS "Use system version of Eigen3 ${EIGEN3_VERSION}")
else()
	set(EIGEN3_VERSION "3.2.8")
	include(resources/eigen3.cmake)
	message(STATUS "Use shipped version of Eigen3 ${EIGEN3_VERSION}")
endif()


##### GiNaC
if(USE_GINAC)
	if(NOT FORCE_SHIPPED_RESOURCES)
		load_library(carl GINAC 1.6.2)
	endif()
	if(GINAC_FOUND)
		message(STATUS "Use system version of GiNaC ${GINAC_VERSION}")
	else()
		set(GINAC_VERSION "1.7.2")
		include(resources/ginac.cmake)
		message(STATUS "Use shipped version of GiNaC ${GINAC_VERSION}")
	endif()
endif()


##### GTest
#if(NOT FORCE_SHIPPED_RESOURCES)
#	load_library(carl GTest 1.6)
#endif()
if(GTEST_FOUND)
	set(GTEST_LIBRARIES ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
	message(STATUS "Use system version of GTest")
else()
	set(GTEST_VERSION "1.8.0")
	message(STATUS "Use shipped version of GTest ${GTEST_VERSION}")
	include(resources/gtest.cmake)
endif()

##### MPFR
IF(USE_MPFR_FLOAT)
	load_library(carl MPFR 0.0 REQUIRED)
endif()

##### Z3
if(COMPARE_WITH_Z3)
    load_library(carl Z3 4.0)
endif()

##### Doxygen
find_package(Doxygen)
if(DOXYGEN)
	add_subdirectory(doxygen-conf)
else()
	message(STATUS "Doxygen not found, doc target is not available.")
endif()
