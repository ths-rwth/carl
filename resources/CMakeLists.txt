add_custom_target(resources)
add_custom_target(install-resources)

# Enable ExternalProject CMake module
INCLUDE(ExternalProject)

# Set default ExternalProject root directory
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/resources)

# Adding log4cplus.
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

	set( LOG4CPLUS_INCLUDE_DIR "${source_dir}/include" PARENT_SCOPE )
	set( LOG4CPLUS_LIBRARY "${binary_dir}/src/liblog4cplus.so.7" PARENT_SCOPE )
	add_dependencies(resources log4cplus)
endif()

if(NOT GTEST_FOUND)
	# Add gtest
	ExternalProject_Add(
		googletest
		#For downloads (may be useful later!)
		#SVN_REPOSITORY http://googletest.googlecode.com/svn/trunk/
		#TIMEOUT 10
		DOWNLOAD_COMMAND ""
		SOURCE_DIR "${CMAKE_SOURCE_DIR}/resources/gtest-1.6.0"
		# Force separate output paths for debug and release builds to allow easy
		# identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
		CMAKE_ARGS -Dgtest_force_shared_crt=ON
		# Disable install step
		INSTALL_COMMAND ""
		# Wrap download, configure and build steps in a script to log output
		LOG_CONFIGURE ON
		LOG_BUILD ON)

	# Specify include dir
	ExternalProject_Get_Property(googletest source_dir)
	set(GTEST_INCLUDE_DIR ${source_dir}/include)
	
	# Specify MainTest's link libraries
	ExternalProject_Get_Property(googletest binary_dir)
	set(GTEST_LIBRARY ${binary_dir}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest${CMAKE_FIND_LIBRARY_SUFFIXES})

	add_dependencies(resources googletest)
endif()
