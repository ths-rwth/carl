include_directories(${GTEST_INCLUDE_DIR} ${CMAKE_BINARY_DIR}/source ${include_dirs})

################################
# Unit Tests
################################
add_subdirectory(core)
