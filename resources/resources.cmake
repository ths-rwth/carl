include(ExternalProject)
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/resources)

add_custom_target(resources)

###############
##### Process resource dependencies
###############
if(USE_GINAC)
	set(USE_CLN_NUMBERS ON)
endif()

###############
##### Load resources
###############

##### Boost
if(NOT FORCE_SHIPPED_RESOURCES)
	load_library(carl Boost 1.55 REQUIRED)
endif()
if(BOOST_FOUND)
	message(STATUS "Use system version of Boost ${BOOST_VERSION}")
else()
	set(BOOST_VERSION "1.60.0")
	message(STATUS "Use shipped version of Boost ${BOOST_VERSION}")
	#include(resources/boost.cmake)
	#link_directories(${BOOST_LIBRARY_DIR})
endif()
include_directories(SYSTEM ${BOOST_INCLUDE_DIRS})
list(APPEND carl_LIBRARIES_DYNAMIC ${BOOST_LIBRARIES_DYNAMIC})
list(APPEND carl_LIBRARIES_STATIC ${BOOST_LIBRARIES_STATIC})


##### CLN
if(USE_CLN_NUMBERS)
	if(NOT FORCE_SHIPPED_RESOURCES)
		#load_library(carl CLN 1.3)
	endif()
	if(CLN_FOUND)
		message(STATUS "Use system version of CLN ${CLN_VERSION}")
	else()
		set(CLN_VERSION "1.3.4")
		include(resources/cln.cmake)
		message(STATUS "Use shipped version of CLN ${CLN_VERSION}")
	endif()
endif()


##### Eigen3
if(NOT FORCE_SHIPPED_RESOURCES)
	load_library(carl Eigen3 3.2)
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
		#load_library(carl GINAC 1.5.8)
	endif()
	if(GINAC_FOUND)
		message(STATUS "Use system version of GiNaC ${GINAC_VERSION}")
	else()
		set(GINAC_VERSION "1.6.7")
		include(resources/ginac.cmake)
		message(STATUS "Use shipped version of GiNaC ${GINAC_VERSION}")
	endif()
endif()


##### GMP / GMPXX
if(NOT FORCE_SHIPPED_RESOURCES)
	load_library(carl GMP 5.1)
	load_library(carl GMPXX 5.1)
endif()
if(GMP_FOUND)
	message(STATUS "Use system version of GMP ${GMP_VERSION}")
else()
	set(GMP_VERSION "6.1.0")
	include(resources/gmp.cmake)
	message(STATUS "Use shipped version of GMP ${GMP_VERSION}")
endif()

##### GTest
if(NOT FORCE_SHIPPED_RESOURCES)
	load_library(carl GTest 1.6)
endif()
if(GTEST_FOUND)
	set(GTEST_LIBRARIES ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
	message(STATUS "Use system version of GTest")
else()
	set(GTEST_VERSION "1.7.0")
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
