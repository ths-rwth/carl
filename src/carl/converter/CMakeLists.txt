# Header files in here (only headers from this directory!)
set(carl_lib_converter_headers 
    GinacConverter.h
) 
# Source files in here
set(carl_lib_converter_sources 
    converter/GinacConverter.cpp
    PARENT_SCOPE
)

# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/carl/converter/config.h.in 
				${CMAKE_SOURCE_DIR}/src/carl/converter/config.h
)  

# Installation of header files
install(FILES			${carl_lib_converter_headers}
		DESTINATION		include/carl/converter
)

