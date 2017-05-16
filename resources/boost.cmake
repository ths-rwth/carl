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

ExternalProject_Get_Property(boost INSTALL_DIR)

add_imported_library(BOOST SHARED "" "${INSTALL_DIR}/include")
link_directories("${INSTALL_DIR}/lib")

add_imported_library(BOOST STATIC "" "${INSTALL_DIR}/include")

add_dependencies(resources boost)
