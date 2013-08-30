# Add test cpp file
add_executable( runGroebnerTests
				Test_Ideal.cpp
				Test_Reductor.cpp
				Test_GB_Buchberger.cpp
			  )
target_link_libraries(runGroebnerTests lib_carl ${libraries} ${GTEST_LIBRARIES})

add_test( NAME groebner COMMAND runGroebnerTests )