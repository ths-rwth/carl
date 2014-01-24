add_executable( runIntervalTests
				Test_DoubleInterval.cpp
                                #Test_FloatInterval.cpp
                                Test_ExactInterval.cpp
                                Test_IntervalEvaluation.cpp
                                Test_Contraction.cpp
			  )
cotire(runIntervalTests)
target_link_libraries(runIntervalTests ${GTEST_LIBRARIES} lib_carl ${libraries})

add_test( NAME interval COMMAND runIntervalTests )