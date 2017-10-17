# Add test cpp file
add_executable( runGroebnerTests
				Test_Ideal.cpp
				Test_Reductor.cpp
				Test_GB_Buchberger.cpp
			  )
cotire(runGroebnerTests)
target_link_libraries(runGroebnerTests TestCommon)

add_test( NAME groebner COMMAND runGroebnerTests )
add_dependencies(all-tests runGroebnerTests)
