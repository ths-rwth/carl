# Add test cpp file
add_executable( runCADTests
    Test_CAD.cpp
    Test_ConflictGraph.cpp
    Test_EliminationSet.cpp
    Test_SampleSet.cpp
)
target_link_libraries(runCADTests ${GTEST_LIBRARIES} lib_carl ${libraries})

add_test( NAME cad COMMAND runCADTests )