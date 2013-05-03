add_custom_target(dependencies)

# Enable ExternalProject CMake module
INCLUDE(ExternalProject)

# Set default ExternalProject root directory
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/resources)

if(NOT LOG4CPLUS_FOUND)	
# Add log4cplus
ExternalProject_Add(
    log4cplus
	DOWNLOAD_COMMAND ""
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/log4cplus-1.1.1"
	# Force separate output paths for debug and release builds to allow easy
    # identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
    # CMAKE_ARGS -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
    #           -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
	# Disable installing..
    INSTALL_COMMAND ""
    # Wrap download, configure and build steps in a script to log output
    LOG_CONFIGURE ON
    LOG_BUILD ON)
	
ExternalProject_Get_Property(log4cplus source_dir)

# Specify MainTest's link libraries
ExternalProject_Get_Property(log4cplus binary_dir)
#TARGET_LINK_LIBRARIES(MainTest
#                      debug ${binary_dir}/DebugLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES}
#                     optimized ${binary_dir}/ReleaseLibs/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES})

	
	set( LOG4CPLUS_INCLUDE_DIR "${source_dir}/include" PARENT_SCOPE )
	set( LOG4CPLUS_LIB "${binary_dir}/src/liblog4cplus.so.7" PARENT_SCOPE )
	add_dependencies(dependencies log4cplus)
endif()


