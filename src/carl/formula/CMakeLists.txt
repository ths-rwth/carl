if(FALSE)
# Add all bitvector-related sources
add_subdirectory(bitvector)

# Header files in here (only headers from this directory!)
set(carl_lib_formula_headers
    Formula.h
    Formula.tpp
    FormulaPool.h
    FormulaPool.tpp
    Condition.h
    Constraint.h
    Constraint.tpp
    ConstraintPool.h
    ConstraintPool.tpp
    Relation.h
    Sort.h
    SortManager.h
    UFInstance.h
    UFInstanceManager.h
    UFManager.h
    UninterpretedFunction.h
)

# Source files in here
set(carl_lib_formula_sources
    ${carl_lib_formula_bitvector_sources}
<<<<<<< HEAD
    formula/UEquality.cpp
    formula/Sort.cpp
    formula/SortManager.cpp
    formula/UFInstance.cpp
    formula/UFInstanceManager.cpp
    formula/UFManager.cpp
    formula/UninterpretedFunction.cpp
=======
    formula/uninterpreted/UEquality.cpp
    formula/Sort.cpp
    formula/SortManager.cpp
    formula/uninterpreted/UFInstance.cpp
    formula/uninterpreted/UFInstanceManager.cpp
    formula/uninterpreted/UFManager.cpp
    formula/uninterpreted/UninterpretedFunction.cpp
>>>>>>> master
    PARENT_SCOPE
)

# Write config.h
configure_file( ${CMAKE_SOURCE_DIR}/src/carl/formula/config.h.in
                ${CMAKE_SOURCE_DIR}/src/carl/formula/config.h
)

# Installation of header files
install(FILES ${carl_lib_formula_headers}
	DESTINATION include/carl/formula
)
<<<<<<< HEAD
endif()
=======
>>>>>>> master
