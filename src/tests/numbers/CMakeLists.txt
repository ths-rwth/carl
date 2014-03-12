# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/tests/numbers/config.h.in 
                ${CMAKE_SOURCE_DIR}/src/tests/numbers/config.h
)  

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
)
cotire(runNumbersTests)
target_link_libraries(runNumbersTests ${GTEST_LIBRARIES} lib_carl ${libraries})

add_test( NAME numbers COMMAND runNumbersTests )