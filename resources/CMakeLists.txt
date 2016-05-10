
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

set(carl_INCLUDE_DIRS "${carl_INCLUDE_DIRS}" PARENT_SCOPE)
set(carl_LIBRARIES_DYNAMIC "${carl_LIBRARIES_DYNAMIC}" PARENT_SCOPE)
set(carl_LIBRARIES_STATIC "${carl_LIBRARIES_STATIC}" PARENT_SCOPE)
