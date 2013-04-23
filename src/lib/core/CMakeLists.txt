# Header files in here (only headers from this directory!)
set(arithmetic_lib_core_headers ) 
# Source files in here
set(arithmetic_lib_core_sources PARENT_SCOPE)

# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/lib/core/config.h.in 
				${CMAKE_SOURCE_DIR}/src/lib/core/config.h
)
# Add config.h to headers.
set( arithmetic_lib_core_headers ${arithmetic_lib_core_headers} config.h )  

# Installation of header files
install(FILES			${arithmetic_lib_core_headers}
		DESTINATION		include/arithmetic/lib/core
)

