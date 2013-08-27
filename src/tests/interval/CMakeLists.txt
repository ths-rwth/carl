add_executable( runIntervalTests
				Test_DoubleInterval.cpp
                                Test_IntervalEvaluation.cpp
			  )
target_link_libraries(runIntervalTests lib_carl ${libraries} ${GTEST_LIBRARIES})

add_test( NAME interval COMMAND runIntervalTests )