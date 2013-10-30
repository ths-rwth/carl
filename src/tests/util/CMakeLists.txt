# Add test cpp file
add_executable( runUtilTests
				Test_SFINAE.cpp
				 )
target_link_libraries(runUtilTests lib_carl ${libraries} ${GTEST_LIBRARIES})

add_test( NAME util COMMAND runUtilTests )