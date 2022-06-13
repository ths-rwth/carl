# Libraries..
add_subdirectory(carl-common)
add_subdirectory(carl-logging)
add_subdirectory(carl-arith)
add_subdirectory(carl-covering)
add_subdirectory(carl-extpolys)
add_subdirectory(carl-io)
add_subdirectory(carl-formula)
add_subdirectory(carl-settings)
add_subdirectory(carl-statistics)

add_custom_target(libs)
add_dependencies(libs carl-common carl-logging carl-arith carl-covering carl-extpolys carl-io carl-formula carl-settings carl-statistics)

# For unit tests.
# We decided to always compile them, if gtest is present,
# since non-compiling tests are a major blocker of development.
add_subdirectory(tests)

# For examples.
add_subdirectory(examples) # EXCLUDE_FROM_ALL

clang_tidy_recurse("${CMAKE_SOURCE_DIR}/src" "")

if(CLANG_TIDY)
	add_custom_target(tidy
		COMMENT "Running clang-tidy"
		DEPENDS tidy-carl tidy-carl-settings
	)
endif()
