cmake_minimum_required(VERSION 3.2)

file(STRINGS defs.hh version REGEX "const version = \".*\";")
string(REGEX MATCH "[0-9]+\\.[0-9]+" version "${version}")
project(bliss VERSION ${version})

# Use correct library extensions
set(DYNAMIC_EXT ".so")
set(STATIC_EXT ".a")
if(APPLE)
	set(DYNAMIC_EXT ".dylib")
	set(STATIC_EXT ".a")
	set(CMAKE_MACOSX_RPATH 1)
elseif (WIN32)
	set(DYNAMIC_EXT ".dll")
	set(STATIC_EXT ".lib")
endif()

# Compiler options
set(CMAKE_CXX_FLAGS "-Wall --pedantic -O4 -fPIC")

# Collect source files
file(GLOB_RECURSE sources "*.cc")
list(REMOVE_ITEM sources ${CMAKE_CURRENT_SOURCE_DIR}/bliss.cc)
file(GLOB_RECURSE headers "*.h*")

# Shared library
add_library(libbliss_shared SHARED ${sources})
set_target_properties(libbliss_shared PROPERTIES
	VERSION "${PROJECT_VERSION}"
	OUTPUT_NAME bliss
)

# Static library
add_library(libbliss_static STATIC ${sources})
set_target_properties(libbliss_static PROPERTIES
	VERSION "${PROJECT_VERSION}"
	OUTPUT_NAME bliss
)

# Install
install(
	FILES ${headers}
	DESTINATION ${CMAKE_INSTALL_PREFIX}/include/bliss
)
install(
	TARGETS libbliss_shared
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
install(
	TARGETS libbliss_static
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
