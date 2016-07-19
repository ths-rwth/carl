add_executable( runIntervalTests
				Test_DoubleInterval.cpp
                                Test_clRA_Interval.cpp
                                Test_MpqInterval.cpp
								Test_MpfrInterval.cpp
                                Test_IntervalEvaluation.cpp
                                Test_Contraction.cpp
								Test_Sample.cpp
			  )
cotire(runIntervalTests)
target_link_libraries(runIntervalTests TestCommon)

add_test( NAME interval COMMAND runIntervalTests )
