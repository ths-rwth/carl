# Add test cpp file
add_executable( runUtilTests
				Test_SFINAE.cpp
				Test_StringParser.cpp
				Test_Parser.cpp
				Test_hash.cpp
				 )
cotire(runUtilTests)
target_link_libraries(runUtilTests ${GTEST_LIBRARIES} lib_carl ${libs_dynamic} prepareTest)

add_test( NAME util COMMAND runUtilTests )
