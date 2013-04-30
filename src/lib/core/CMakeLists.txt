# Header files in here (only headers from this directory!)
set(arithmetic_lib_core_headers 
	AbstractPolynomial.h
	Polynomial.h
	PolynomialPolicy.h
	Variable.h
	VariablePool.h
	initialize.h
	logging.h
	config.h
) 
# Source files in here
set(arithmetic_lib_core_sources 
	core/VariablePool.cpp
PARENT_SCOPE)

# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/lib/core/config.h.in 
				${CMAKE_SOURCE_DIR}/src/lib/core/config.h
)  

# Installation of header files
install(FILES			${arithmetic_lib_core_headers}
		DESTINATION		include/arithmetic/lib/core
)

