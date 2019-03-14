# Libraries..
add_subdirectory(carl)
add_subdirectory(carl-settings)

# For unit tests.
# We decided to always compile them, if gtest is present,
# since non-compiling tests are a major blocker of development.
add_subdirectory(tests)

# For examples.
add_subdirectory(examples EXCLUDE_FROM_ALL)

clang_tidy_recurse("${CMAKE_SOURCE_DIR}/src" "")

if(CLANG_TIDY)
	add_custom_target(tidy
		COMMENT "Running clang-tidy"
		DEPENDS tidy-carl tidy-carl-settings
	)
endif()
