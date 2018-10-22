#pragma once

#include "../RealAlgebraicNumber.h"

namespace carl {

template<typename Number>
RealAlgebraicNumber<Number> evaluateZ3(const MultivariatePolynomial<Number>& polynomial, const std::map<Variable, RealAlgebraicNumber<Number>>& evalMap);

}