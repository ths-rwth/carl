set(carl_lib_number_headers 
	GF.h
	PrimeFactory.h
) 
# Source files in here
set(carl_lib_number_sources 
	numbers/GF.cpp
	
PARENT_SCOPE)

# Installation of header files
install(FILES			${carl_lib_interval_headers}
		DESTINATION		include/carl/numbers
)
