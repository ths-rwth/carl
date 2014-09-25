add_executable( runIntervalTests
				Test_DoubleInterval.cpp
                                Test_clRA_Interval.cpp
                                Test_MpqInterval.cpp
                                Test_IntervalEvaluation.cpp
                                Test_Contraction.cpp
			  )
cotire(runIntervalTests)
target_link_libraries(runIntervalTests ${GTEST_LIBRARIES} lib_carl ${libs_dynamic})

add_test( NAME interval COMMAND runIntervalTests )