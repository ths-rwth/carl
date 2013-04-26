# Start compiling resources (if necessary)
add_subdirectory(resources)

# Libraries..
set( libraries ${LOG4CPLUS_LIB} )

message( ${libraries})
add_subdirectory(lib)

# For unit tests.
if(CppUnit_FOUND)
    add_subdirectory(tests EXCLUDE_FROM_ALL)
endif()

# For examples.
add_subdirectory(examples EXCLUDE_FROM_ALL)