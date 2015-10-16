# Add test cpp file
add_executable(runFormulaTests
	Test_Formula.cpp
	Test_BVConstraint.cpp
)
cotire(runFormulaTests)

target_link_libraries(runFormulaTests ${GTEST_LIBRARIES} lib_carl ${libs_dynamic} prepareTest)

cotire(runFormulaTests)

add_test( NAME formula COMMAND runFormulaTests )
