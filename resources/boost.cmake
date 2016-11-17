message(STATUS "Get actual version of Boost and install it in resources")
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
	GIT_SUBMODULES 
		libs/algorithm
		libs/config
		libs/dynamic_bitset
		libs/filesystem
		libs/functional
		libs/fusion
		libs/numeric/interval
		libs/optional 
		libs/phoenix
		libs/program_options
		libs/spirit
		libs/system
		libs/variant
		tools/build
		tools/inspect 
	GIT_REPOSITORY https://github.com/boostorg/boost.git
	GIT_TAG "boost-${BOOST_VERSION}"
	BUILD_IN_SOURCE 1
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	CONFIGURE_COMMAND ${Boost_Bootstrap_Command} --prefix=<INSTALL_DIR>
	BUILD_COMMAND  ${Boost_b2_Command} --variant=release headers
	INSTALL_COMMAND ${Boost_b2_Command} --variant=release install
)

ExternalProject_Get_Property(boost INSTALL_DIR)

add_imported_library(BOOST SHARED "" "${INSTALL_DIR}/include")
link_directories("${INSTALL_DIR}/lib")

add_dependencies(resources boost)
