# Add test cpp file
message(${libraries})
add_executable( runCoreTests
				Test_Variable.cpp 
				Test_VariablePool.cpp 
				Test_Monomial.cpp
				Test_Term.cpp
				Test_MultivariatePolynomial.cpp
				Test_UnivariatePolynomial.cpp
				
				 )
target_link_libraries(runCoreTests lib_carl ${libraries} ${GTEST_LIBRARIES})

add_test( NAME core COMMAND runCoreTests )