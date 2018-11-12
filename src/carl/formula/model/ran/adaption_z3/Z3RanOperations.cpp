#include "../../../../converter/Z3Converter.h"

#ifdef USE_Z3_RANS

#include "Z3RanOperations.h"
#include "Z3RanContent.h"

namespace carl {

template<typename Number>
RealAlgebraicNumber<Number> sampleBelowZ3(const RealAlgebraicNumber<Number>& n) {
    algebraic_numbers::anum val;
    z3().anumMan().int_lt(n.getZ3Ran().content(), val);
    return RealAlgebraicNumber<Number>(Z3Ran<Number>(std::move(val)));
}

template<typename Number>
RealAlgebraicNumber<Number> sampleBetweenZ3(const RealAlgebraicNumber<Number>& lower, const RealAlgebraicNumber<Number>& upper) {
    algebraic_numbers::anum val;
    if (lower.isZ3Ran() && upper.isZ3Ran()) {
        z3().anumMan().select(lower.getZ3Ran().content(), upper.getZ3Ran().content(), val);        
    } else if (lower.isZ3Ran() && upper.isNumeric()) {
        mpq num = z3().toZ3MPQ(upper.value());
        anum alnum;
        z3().anumMan().set(alnum, num);
        z3().anumMan().select(lower.getZ3Ran().content(), alnum, val); 
        z3().free(num);
        z3().free(alnum);

    } else if (lower.isNumeric() && upper.isZ3Ran()) {
        mpq num = z3().toZ3MPQ(lower.value());
        anum alnum;
        z3().anumMan().set(alnum, num);
        z3().anumMan().select(alnum, upper.getZ3Ran().content(), val); 
        z3().free(num);
        z3().free(alnum);
    } else {
        assert(false);
    }
    return RealAlgebraicNumber<Number>(Z3Ran<Number>(std::move(val)));
}

template<typename Number>
RealAlgebraicNumber<Number> sampleAboveZ3(const RealAlgebraicNumber<Number>& n) {
    algebraic_numbers::anum val;
    z3().anumMan().int_gt(n.getZ3Ran().content(), val);
    return RealAlgebraicNumber<Number>(Z3Ran<Number>(std::move(val)));
}


template RealAlgebraicNumber<mpq_class> sampleBelowZ3(const RealAlgebraicNumber<mpq_class>& n);
template RealAlgebraicNumber<mpq_class> sampleBetweenZ3(const RealAlgebraicNumber<mpq_class>& lower, const RealAlgebraicNumber<mpq_class>& upper);
template RealAlgebraicNumber<mpq_class> sampleAboveZ3(const RealAlgebraicNumber<mpq_class>& n);

}

#endif