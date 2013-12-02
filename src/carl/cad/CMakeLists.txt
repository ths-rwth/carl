# Header files in here (only headers from this directory!)
set(carl_lib_cad_headers 
	EliminationSet.h
) 
# Source files in here
set(carl_lib_cad_sources 
PARENT_SCOPE)

# Write config.h 
#configure_file( ${CMAKE_SOURCE_DIR}/src/carl/cad/config.h.in 
#				${CMAKE_SOURCE_DIR}/src/carl/cad/config.h
#)  

# Installation of header files
install(FILES			${carl_lib_cad_headers}
		DESTINATION		include/carl/cad
)

