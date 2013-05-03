#ADD_SUBDIRECTORY (gtest-1.6.0)
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${GTEST_INCLUDE_DIR})


################################
# Unit Tests
################################
# Add test cpp file
add_executable( runUnitTests test.cpp )
# Link test executable against gtest & gtest_main
#target_link_libraries(runUnitTests gtest gtest_main)
target_link_libraries(runUnitTests ${GTEST_LIBRARY} ${GTEST_MAIN_LIBRARY})
add_test( NAME sample COMMAND runUnitTests )