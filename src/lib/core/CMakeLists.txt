# Header files in here (only headers from this directory!)
set(arithmetic_lib_core_headers ) 
# Source files in here
set(arithmetic_lib_core_sources PARENT_SCOPE)


# Installation of header files
install(FILES ${arithmetic_lib_core_headers}
		DESTINATION include/arithmetic/lib/core )