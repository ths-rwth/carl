# Add test cpp file
add_executable(runFormulaTests
	Test_BVConstraint.cpp
	Test_BVTerm.cpp
	Test_BVTermType.cpp
	Test_BVVariable.cpp
	Test_Formula.cpp
	Test_Model.cpp
	Test_ModelEvaluation.cpp
	Test_OPBParser.cpp
	Test_PBConstraint.cpp
	Test_Sort.cpp
)
cotire(runFormulaTests)

target_link_libraries(runFormulaTests TestCommon)

cotire(runFormulaTests)

add_test( NAME formula COMMAND runFormulaTests )
