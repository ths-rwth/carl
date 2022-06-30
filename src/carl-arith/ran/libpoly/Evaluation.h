#pragma once

#include <carl-common/config.h>
#ifdef USE_LIBPOLY

#include "LPRan.h"
#include "carl-arith/poly/lp/LPPolynomial.h"

namespace carl {

std::optional<RealAlgebraicNumberLibpoly> evaluate(const LPPolynomial& polynomial,const std::map<Variable, RealAlgebraicNumberLibpoly>& evalMap);
boost::tribool evaluate(const BasicConstraint<LPPolynomial>& constraint, const std::map<Variable, RealAlgebraicNumberLibpoly>& evalMap);

} // namespace carl

#endif