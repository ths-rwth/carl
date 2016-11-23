# Add test cpp file
add_executable( runUtilTests
				Test_SFINAE.cpp
				Test_StringParser.cpp
				Test_Parser.cpp
				Test_hash.cpp
				Test_Timer.cpp
				 )
cotire(runUtilTests)
target_link_libraries(runUtilTests TestCommon)

add_test( NAME util COMMAND runUtilTests )
