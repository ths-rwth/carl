# Add test cpp file
add_executable(runFormulaTests
	Test_Formula.cpp
	Test_BVConstraint.cpp
	Test_Model.cpp
	Test_ModelEvaluation.cpp
	Test_OPBParser.cpp
)
cotire(runFormulaTests)

target_link_libraries(runFormulaTests TestCommon)

cotire(runFormulaTests)

add_test( NAME formula COMMAND runFormulaTests )
