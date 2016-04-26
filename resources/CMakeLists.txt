add_custom_target(resources)
add_custom_target(install-resources)

# Enable ExternalProject CMake module
INCLUDE(ExternalProject)

# Set default ExternalProject root directory
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/resources)

if(GTEST_FOUND)
	set(GTEST_LIBRARIES ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} PARENT_SCOPE)
	message("-- Use system version of GTest")
else()
	message("-- Use shipped version of GTest")
	include(gtest.cmake)
	set(GTEST_LIBRARIES ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY} PARENT_SCOPE)
	set(GTEST_INCLUDE_DIR ${GTEST_INCLUDE_DIR} PARENT_SCOPE)
endif()

if(EIGEN3_FOUND)
	message(STATUS "Use system version of Eigen3")
else()
	message(STATUS "Use shipped version of Eigen3")
	ExternalProject_Add(
		eigen
		DOWNLOAD_COMMAND ""
		SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/eigen-3.2.0"
		CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_SOURCE_DIR}/resources/eigen-3.2.0/build
		#		   -DBUILD_SHARED_LIBS:BOOL=0
		INSTALL_DIR ${CMAKE_SOURCE_DIR}/resources/eigen-3.2.0/build
		# Wrap download, configure and build steps in a script to log output
		LOG_CONFIGURE ON
		LOG_BUILD ON)


	set( Eigen3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/resources/eigen-3.2.0/build/include")
	message(STATUS "Found Eigen3: ${Eigen3_INCLUDE_DIR}")
	set(carl_INCLUDE_DIRS "${carl_INCLUDE_DIRS};${Eigen3_INCLUDE_DIR}" PARENT_SCOPE)
	add_dependencies(resources eigen)
endif()

if(OBTAIN_BOOST)
	message("-- Get actual version of Boost and install it in resources")
	set( Boost_Bootstrap_Command )
	if( UNIX )
		set( Boost_Bootstrap_Command ./bootstrap.sh )
		set( Boost_b2_Command ./b2 )
	else()
		if( WIN32 )
			set( Boost_Bootstrap_Command bootstrap.bat )
			set( Boost_b2_Command b2.exe )
		endif()
	endif()

	ExternalProject_Add(
	boost
	GIT_SUBMODULES tools/build tools/inspect libs/numeric/interval
	GIT_REPOSITORY https://github.com/boostorg/boost.git
	INSTALL_DIR ${CMAKE_SOURCE_DIR}/resources/boost
	BUILD_IN_SOURCE 1
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	CONFIGURE_COMMAND ${Boost_Bootstrap_Command}
	BUILD_COMMAND  ${Boost_b2_Command}
	--without-python
    --without-mpi
    --disable-icu
    --prefix=${CMAKE_SOURCE_DIR}/resources/boost
    --threading=single,multi
    --link=shared
    --variant=release
    -j8
	INSTALL_COMMAND ""
)


  if( NOT WIN32 )
    set(Boost_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/build/resources/src/boost/stage/lib )
    set(Boost_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/build/resources/src/boost/ )
  else()
    set(Boost_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/resources/boost/lib/ )
    set(Boost_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/resources/boost/include/ )
  endif()

  add_dependencies(resources boost)
endif()
