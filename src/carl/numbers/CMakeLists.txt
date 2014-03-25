set(carl_lib_number_headers 
	GF.h
	PrimeFactory.h
        FLOAT_T.h
        Numeric.h
        numbers.h
        operations.h
        operations_cln.h
        operations_gmp.h
        operations_native.h
        operations_mpfr.h
        typetraits.h
        roundingConversion.h
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

# Collect headers needed for float_t TODO: Clear the floatTypes directory first?
#set(float_t_headers)
#if(USE_MPFR_FLOAT)
#    set(float_t_headers ${float_t_headers} floatTypes/mpfr_float.tpp)
#endif()

# Installation of float_t files needed
#install(FILES                   FLOAT_T.h
#                DESTINATION             include/carl/numbers)
#install(FILES                   ${float_t_headers}
#                DESTINATION             include/carl/numbers/floatTypes)
