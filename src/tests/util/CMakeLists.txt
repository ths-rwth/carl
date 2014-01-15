# Add test cpp file
add_executable( runUtilTests
				Test_SFINAE.cpp
				 )
cotire(runUtilTests)
target_link_libraries(runUtilTests ${GTEST_LIBRARIES} lib_carl ${libraries})

add_test( NAME util COMMAND runUtilTests )