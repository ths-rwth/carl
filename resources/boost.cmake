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
	Boost-EP
	GIT_SUBMODULES 
		libs/algorithm
		libs/any
		libs/array
		libs/assert
		libs/bind
		libs/concept_check
		libs/config
		libs/container
		libs/core
		libs/detail
		libs/dynamic_bitset
		libs/filesystem
		libs/foreach
		libs/format
		libs/function
		libs/function_types
		libs/functional
		libs/fusion
		libs/integer
		libs/intrusive
		libs/io
		libs/iostreams
		libs/iterator
		libs/lexical_cast
		libs/math
		libs/move
		libs/mpl
		libs/numeric/conversion
		libs/numeric/interval
		libs/optional 
		libs/phoenix
		libs/predef
		libs/preprocessor
		libs/program_options
		libs/proto
		libs/range
		libs/regex
		libs/smart_ptr
		libs/spirit
		libs/static_assert
		libs/system
		libs/throw_exception
		libs/tokenizer
		libs/type_index
		libs/type_traits
		libs/typeof
		libs/utility
		libs/variant
		tools/build
		tools/inspect 
	GIT_REPOSITORY https://github.com/boostorg/boost.git
	GIT_TAG "boost-${BOOST_VERSION}"
	BUILD_IN_SOURCE 1
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	CONFIGURE_COMMAND ${Boost_Bootstrap_Command} --prefix=<INSTALL_DIR>
	BUILD_COMMAND  ${Boost_b2_Command} -s NO_BZIP2=1 --variant=release headers
	INSTALL_COMMAND ${Boost_b2_Command} -s NO_BZIP2=1 --variant=release install
)

ExternalProject_Get_Property(Boost-EP INSTALL_DIR)

set(shared_libs)
set(static_libs)
foreach(component ${BOOST_COMPONENTS})
	list(APPEND shared_libs "${INSTALL_DIR}/lib/lib${component}${DYNAMIC_EXT}")
	list(APPEND static_libs "${INSTALL_DIR}/lib/lib${component}${STATIC_EXT}")
endforeach()

add_imported_library(Boost SHARED "${shared_libs}" "${INSTALL_DIR}/include" ${BOOST_COMPONENTS})
add_imported_library(Boost STATIC "${static_libs}" "${INSTALL_DIR}/include" ${BOOST_COMPONENTS})

unset(shared_libs)
unset(static_libs)

add_dependencies(Boost_SHARED Boost-EP)
add_dependencies(Boost_STATIC Boost-EP)
add_dependencies(resources Boost_SHARED Boost_STATIC)
