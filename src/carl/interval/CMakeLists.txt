set(carl_lib_interval_headers 
	DoubleInterval.h
) 
# Source files in here
set(carl_lib_interval_sources 
	interval/DoubleInterval.cpp
	
PARENT_SCOPE)

# Installation of header files
install(FILES			${carl_lib_interval_headers}
		DESTINATION		include/carl/interval
)
