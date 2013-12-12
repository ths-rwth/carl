set(carl_lib_number_headers 
	GF.h
	PrimeFactory.h
        numbers.h
        operations.h
        operations_cln.h
        operations_gmp.h
        operations_native.h
        typetraits.h
) 
# Source files in here
set(carl_lib_number_sources 
	
	
PARENT_SCOPE)

# Installation of header files
install(FILES			${carl_lib_interval_headers}
		DESTINATION		include/carl/numbers
)
