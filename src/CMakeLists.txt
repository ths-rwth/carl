add_subdirectory(lib)

# For unit tests.
if(CppUnit_FOUND)
    add_subdirectory(test EXCLUDE_FROM_ALL)
endif()

# For examples.
add_subdirectory(examples EXCLUDE_FROM_ALL)