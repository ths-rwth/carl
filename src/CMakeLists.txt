# Libraries..
add_subdirectory(arithmetic)

# For unit tests.
# We decided to always compile them, since not compiling tests are a major blocker of development.
add_subdirectory(tests)


# For examples.
add_subdirectory(examples EXCLUDE_FROM_ALL)