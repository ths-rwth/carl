# Add test cpp file
add_executable( runNumbersTests
        Test_numbers_cln.cpp
        Test_numbers_gmp.cpp
        Test_numbers_native.cpp
        Test_EEA.cpp
        Test_PrimeFactory.cpp 
        Test_GF.cpp
)
target_link_libraries(runNumbersTests ${GTEST_LIBRARIES} lib_carl ${libraries})

add_test( NAME numbers COMMAND runNumbersTests )