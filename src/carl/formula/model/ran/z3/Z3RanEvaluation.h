#pragma once

#include "Z3Ran.h"

#ifdef RAN_USE_Z3

#include "../RealAlgebraicNumber.h"

namespace carl {

//template<typename Number>
//RealAlgebraicNumber<Number> evaluateZ3(const MultivariatePolynomial<Number>& polynomial, const std::map<Variable, RealAlgebraicNumber<Number>>& evalMap);

template<typename Number>
ran::Z3Content<Number> evaluateZ3(const MultivariatePolynomial<Number>& polynomial, const std::map<Variable, ran::Z3Content<Number>>& evalMap);

}

#endif