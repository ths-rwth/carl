# Libraries..
add_subdirectory(arithmetic)

# For unit tests.
if(CppUnit_FOUND)
    add_subdirectory(tests EXCLUDE_FROM_ALL)
endif()

# For examples.
add_subdirectory(examples EXCLUDE_FROM_ALL)