# Add test cpp file
add_executable( runNumbersTests
    Test_CLN.cpp
    Test_GMP.cpp
    Test_numbers.cpp
    Test_numbers_native.cpp
    Test_Numeric.cpp
    Test_EEA.cpp
    Test_PrimeFactory.cpp
    Test_GF.cpp
    Test_DoubleFloat.cpp
    Test_MpfrFloat.cpp
	Test_operations_division.cpp
	Test_operations_properties.cpp
	Test_operations_rounding.cpp
	Test_stdhash.cpp
	Test_typetraits.cpp
)
cotire(runNumbersTests)
target_link_libraries(runNumbersTests TestCommon)

add_test( NAME numbers COMMAND runNumbersTests )
