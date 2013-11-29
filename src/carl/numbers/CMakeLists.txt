set(carl_lib_number_headers 
	GF.h
	PrimeFactory.h
        FLOAT_T.h
) 

# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/carl/numbers/config.h.in 
				${CMAKE_SOURCE_DIR}/src/carl/numbers/config.h
)  

# Source files in here
set(carl_lib_number_sources 
	
	
PARENT_SCOPE)

# Installation of header files
install(FILES			${carl_lib_interval_headers}
		DESTINATION		include/carl/numbers
)
