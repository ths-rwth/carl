set(carl_lib_interval_headers 
        Contraction.h	
        DoubleInterval.h
        ExactInterval.h
	    IntervalEvaluation.h
        PointIntervalFactory.h
        Interval.h
) 
# Source files in here
set(carl_lib_interval_sources 
	interval/DoubleInterval.cpp

PARENT_SCOPE)

configure_file( config.h.in config.h )

# Installation of header files
install(FILES			${carl_lib_interval_headers}
		DESTINATION		include/carl/interval
)
