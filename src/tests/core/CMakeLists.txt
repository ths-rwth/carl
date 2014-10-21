# Add test cpp file
add_executable(runCoreTests
		Test_OperatorConcepts.cpp
        Test_Variable.cpp 
        Test_VariablePool.cpp 
        Test_Monomial.cpp
        Test_Term.cpp
        Test_MultivariatePolynomial.cpp
        Test_UnivariatePolynomial.cpp
        Test_MultivariateHensel.cpp
        Test_MultivariateGCD.cpp
        Test_RealAlgebraicNumber.cpp
        Test_RootFinder.cpp
        Test_Sign.cpp
        Test_FactorizedPolynomial.cpp
		Test_RationalFunction.cpp
		Test_Constraint.cpp
)
cotire(runCoreTests)

target_link_libraries(runCoreTests ${GTEST_LIBRARIES} lib_carl ${libs_dynamic})

cotire(runCoreTests)

add_test( NAME core COMMAND runCoreTests )
