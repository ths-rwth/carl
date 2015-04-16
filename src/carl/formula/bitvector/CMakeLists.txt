# Header files in here (only headers from this directory!)
set(carl_lib_formula_bitvector_headers
    BVCompareRelation.h
    BVConstraint.h
    BVConstraintPool.h
    BVTerm.h
    BVTermPool.h
    BVVariable.h
    Pool.h
)

# Source files in here
set(carl_lib_formula_bitvector_sources
    formula/bitvector/BVConstraint.cpp
    formula/bitvector/BVConstraintPool.cpp
    formula/bitvector/BVTerm.cpp
    formula/bitvector/BVTermPool.cpp
    PARENT_SCOPE
)

# Installation of header files
install(FILES ${carl_lib_formula_bitvector_headers}
	DESTINATION include/carl/formula/bitvector
)

