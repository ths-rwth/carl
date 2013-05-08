include_directories(${GTEST_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/src ${include_dirs})



# Add test cpp file
add_executable( runCoreTests
				Test_Monomial.cpp
				Test_Term.cpp
				Test_Variable.cpp 
				Test_VariablePool.cpp  )
target_link_libraries(runCoreTests lib_arithmetic ${libraries} ${GTEST_LIBRARIES})

add_test( NAME core COMMAND runCoreTests )