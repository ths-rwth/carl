# Add test cpp file
add_executable( runGroebnerTests
				Test_Ideal.cpp
				Test_Reductor.cpp
				Test_GB_Buchberger.cpp
			  )
cotire(runGroebnerTests)
target_link_libraries(runGroebnerTests ${GTEST_LIBRARIES} lib_carl ${libs_dynamic})

add_test( NAME groebner COMMAND runGroebnerTests )