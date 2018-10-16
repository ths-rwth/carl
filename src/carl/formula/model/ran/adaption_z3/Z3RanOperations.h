#pragma once

#include "Z3Ran.h"


namespace carl {

template<typename Number>
RealAlgebraicNumber<Number> sampleBelowZ3(const RealAlgebraicNumber<Number>& n) {
    algebraic_numbers::anum val;
    z3().anumMan().int_lt(n.getZ3Ran().content(), val);
    return RealAlgebraicNumber<Number>(Z3Ran<Number>(val));
}

template<typename Number>
RealAlgebraicNumber<Number> sampleBetweenZ3(const RealAlgebraicNumber<Number>& lower, const RealAlgebraicNumber<Number>& upper) {
    algebraic_numbers::anum val;
    z3().anumMan().select(lower.getZ3Ran().content(), upper.getZ3Ran().content(), val);
    return RealAlgebraicNumber<Number>(Z3Ran<Number>(val));
}

template<typename Number>
RealAlgebraicNumber<Number> sampleAboveZ3(const RealAlgebraicNumber<Number>& n) {
    algebraic_numbers::anum val;
    z3().anumMan().int_gt(n.getZ3Ran().content(), val);
    return RealAlgebraicNumber<Number>(Z3Ran<Number>(val));
}

}