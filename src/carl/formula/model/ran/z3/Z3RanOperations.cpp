#include "../../../../converter/Z3Converter.h"

#ifdef USE_Z3_RANS

#include "Z3RanOperations.h"
#include "Z3RanContent.h"

namespace carl {

template<typename Number>
Z3Ran<Number> sampleBelowZ3(const Z3Ran<Number>& n) {
    algebraic_numbers::anum val;
    z3().anumMan().int_lt(n.content(), val);
    return Z3Ran<Number>(std::move(val));
}

template<typename Number>
Z3Ran<Number> sampleBetweenZ3(const Z3Ran<Number>& lower, const Z3Ran<Number>& upper) {
    algebraic_numbers::anum val;
    z3().anumMan().select(lower.content(), upper.content(), val);
    return Z3Ran<Number>(std::move(val));
}

template<typename Number>
Z3Ran<Number> sampleAboveZ3(const Z3Ran<Number>& n) {
    algebraic_numbers::anum val;
    z3().anumMan().int_gt(n.content(), val);
    return Z3Ran<Number>(std::move(val));
}

template Z3Ran<mpq_class> sampleBelowZ3(const Z3Ran<mpq_class>& n);
template Z3Ran<mpq_class> sampleBetweenZ3(const Z3Ran<mpq_class>& lower, const Z3Ran<mpq_class>& upper);
template Z3Ran<mpq_class> sampleAboveZ3(const Z3Ran<mpq_class>& n);

}

#endif