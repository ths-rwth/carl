# Add test cpp file
add_executable( runCADTests
				Test_EliminationSet.cpp
			  )
target_link_libraries(runCADTests ${GTEST_LIBRARIES} lib_carl ${libraries})

add_test( NAME cad COMMAND runCADTests )