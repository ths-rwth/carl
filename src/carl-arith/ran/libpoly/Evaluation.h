#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include "LPRan.h"
#include "carl-arith/poly/libpoly/LPPolynomial.h"

namespace carl {

std::optional<LPRealAlgebraicNumber> evaluate(const LPPolynomial& polynomial,const std::map<Variable, LPRealAlgebraicNumber>& evalMap);
boost::tribool evaluate(const BasicConstraint<LPPolynomial>& constraint, const std::map<Variable, LPRealAlgebraicNumber>& evalMap);

} // namespace carl

#endif