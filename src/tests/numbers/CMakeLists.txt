# Add test cpp file
add_executable( runNumbersTests
    Test_numbers_cln.cpp
    Test_numbers_gmpxx.cpp
    Test_numbers_native.cpp
    Test_Numeric.cpp
    Test_EEA.cpp
    Test_PrimeFactory.cpp
    Test_GF.cpp
    Test_DoubleFloat.cpp
    Test_MpfrFloat.cpp
    Test_typetraits.cpp
	Test_operations_division.cpp
	Test_operations_properties.cpp
	Test_operations_rounding.cpp
)
cotire(runNumbersTests)
target_link_libraries(runNumbersTests ${GTEST_LIBRARIES} lib_carl ${libs_dynamic} prepareTest)

add_test( NAME numbers COMMAND runNumbersTests )
