# if log4cplus is not found, we compile this version.
if(NOT LOG4CPLUS_FOUND)
	add_subdirectory(log4cplus)
	set( LOG4CPLUS_INCLUDE_DIR "${CMAKE_BINARY_DIR}/src/resources/log4cplus" PARENT_SCOPE )
	set( LOG4CPLUS_LIB "${CMAKE_BINARY_DIR}/src/resources/log4cplus/src/liglog4cplus.a" PARENT_SCOPE )
endif()

