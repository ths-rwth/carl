# find include dir
string(REPLACE ":" ";" CPATH "$ENV{CPATH}")
find_path(Boost_INCLUDE_DIR
	NAMES boost/version.hpp
	HINTS ${CPATH}
	PATHS /usr/include
	DOC "Include directory for Boost"
)

if (NOT Boost_INCLUDE_DIR)
	message(STATUS "Did not find Boost!")
	return()
endif()

# create umbrella targets Boost_SHARED and Boost_STATIC
add_library(Boost_SHARED INTERFACE IMPORTED GLOBAL)
set_target_properties(Boost_SHARED PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIR}")
add_library(Boost_STATIC INTERFACE IMPORTED GLOBAL)
set_target_properties(Boost_STATIC PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIR}")

# search for every component
foreach(component ${Boost_FIND_COMPONENTS})
	# search for shared variant
	set(CMAKE_FIND_LIBRARY_SUFFIXES "${DYNAMIC_EXT}")
	unset(lib_shared CACHE)
	find_library(lib_shared
		NAMES boost_${component}
		HINTS ${LIBRARY_PATH}
		PATHS /usr/local/lib
	)
	if (NOT lib_shared)
		message(WARNING "Failed to find shared version of Boost component ${component}")
	endif()
	add_library(Boost_${component}_SHARED SHARED IMPORTED GLOBAL)
	set_target_properties(Boost_${component}_SHARED PROPERTIES IMPORTED_LOCATION "${lib_shared}")
	set_target_properties(Boost_${component}_SHARED PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIR}")
	target_link_libraries(Boost_SHARED INTERFACE Boost_${component}_SHARED)

	# search for static variant
	set(CMAKE_FIND_LIBRARY_SUFFIXES "${STATIC_EXT}")
	unset(lib_static CACHE)
	find_library(lib_static
		NAMES boost_${component}
		HINTS ${LIBRARY_PATH}
		PATHS /usr/local/lib
	)
	if (NOT lib_static)
		message(WARNING "Failed to find static version of Boost component ${component}")
	endif()
	add_library(Boost_${component}_STATIC STATIC IMPORTED GLOBAL)
	set_target_properties(Boost_${component}_STATIC PROPERTIES IMPORTED_LOCATION "${lib_static}")
	set_target_properties(Boost_${component}_STATIC PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIR}")
	target_link_libraries(Boost_STATIC INTERFACE Boost_${component}_STATIC)
endforeach()
unset(lib_shared CACHE)
unset(lib_static CACHE)

if(Boost_INCLUDE_DIR)
	set(Boost_FOUND TRUE)
	# parse version from version.hpp
	file(STRINGS "${Boost_INCLUDE_DIR}/boost/version.hpp" Boost_VERSION REGEX "BOOST_VERSION [0-9]+")
	string(REGEX MATCH "[0-9]+" Boost_VERSION "${Boost_VERSION}")
	math(EXPR Boost_VERSION_MAJOR "${Boost_VERSION} / 100000")
	math(EXPR Boost_VERSION_MINOR "${Boost_VERSION} / 100 % 1000")
	math(EXPR Boost_VERSION_PATCH "${Boost_VERSION} % 100")
	set(Boost_VERSION "${Boost_VERSION_MAJOR}.${Boost_VERSION_MINOR}.${Boost_VERSION_PATCH}")

	if(Boost_FIND_VERSION VERSION_GREATER Boost_VERSION)
		message(WARNING "Required Boost ${Boost_FIND_VERSION} but found only Boost ${Boost_VERSION}.")
		return()
	endif()
endif()