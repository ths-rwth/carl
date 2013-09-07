# Add test cpp file
add_executable( runNumbersTests
				Test_PrimeFactory.cpp 
				
				 )
target_link_libraries(runNumbersTests lib_carl ${libraries} ${GTEST_LIBRARIES})

add_test( NAME numbers COMMAND runNumbersTests )