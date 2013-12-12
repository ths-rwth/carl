# Header files in here (only headers from this directory!)
set(carl_lib_core_headers 
    AbstractPolynomial.h
    Monomial.h
    Polynomial.h Polynomial.tpp
    PolynomialPolicy.h
    RealAlgebraicNumber.h
    RealAlgebraicNumberNR.h
    RealAlgebraicNumberIR.h
    RealAlgebraicNumberIR.tpp
    VarExpPair.h
    Variable.h
    VariablePool.h
    initialize.h
    logging.h
    config.h
) 
# Source files in here
set(carl_lib_core_sources 
    core/VariablePool.cpp
    core/Variable.cpp
    core/logging.cpp
PARENT_SCOPE)

# Write config.h 
configure_file( ${CMAKE_SOURCE_DIR}/src/carl/core/config.h.in 
    ${CMAKE_SOURCE_DIR}/src/carl/core/config.h
)  

# Installation of header files
install(FILES			${carl_lib_core_headers}
    DESTINATION		include/carl/core
)

