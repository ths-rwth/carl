# Add test cpp file
add_executable( runCADTests
    Test_CAD.cpp
    #Test_ConflictGraph.cpp
    #Test_Constraint.cpp
    #Test_EliminationSet.cpp
    #Test_SampleSet.cpp
    #Test_TarskiQuery.cpp
    #Test_SignDetermination.cpp
    #Test_Thom.cpp
    Test_Thom2.cpp
)
cotire(runCADTests)

target_link_libraries(runCADTests TestCommon)

add_test( NAME cad COMMAND runCADTests )
