# Add test cpp file
add_executable( runCADTests
    Test_CAD.cpp
    Test_ConflictGraph.cpp
    Test_Constraint.cpp
    Test_EliminationSet.cpp
    Test_SampleSet.cpp
    Test_TarskiQuery.cpp
    #Test_SignDetermination.cpp
    Test_Thom.cpp
)
cotire(runCADTests)

target_link_libraries(runCADTests ${GTEST_LIBRARIES} lib_carl ${libs_dynamic} prepareTest)

add_test( NAME cad COMMAND runCADTests )
