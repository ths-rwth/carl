# Libraries..
add_subdirectory(carl)

# For unit tests.
# We decided to always compile them by default and if gtest is present,
# since non-compiling tests are a major blocker of development.
if (EXCLUDE_TESTS_FROM_ALL)
    add_subdirectory(tests EXCLUDE_FROM_ALL)
else()
    add_subdirectory(tests)
endif()

# For examples.
add_subdirectory(examples EXCLUDE_FROM_ALL)
