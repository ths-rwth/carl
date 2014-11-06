set(carl_lib_interval_headers 
        Contraction.h	
        #DoubleInterval.h
	IntervalEvaluation.h
        #PointIntervalFactory.h
        Interval.h
) 
# Source files in here
set(carl_lib_interval_sources 
	#interval/DoubleInterval.cpp

PARENT_SCOPE)


# Write config.h
configure_file( ${CMAKE_SOURCE_DIR}/src/carl/interval/config.h.in
     ${CMAKE_SOURCE_DIR}/src/carl/interval/config.h
)

# Installation of header files
install(FILES			${carl_lib_interval_headers}
		DESTINATION		include/carl/interval
)
