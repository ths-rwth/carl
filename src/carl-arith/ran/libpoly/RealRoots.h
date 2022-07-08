#pragma once

#include <carl-common/config.h>

#ifdef USE_LIBPOLY

#include "../real_roots_common.h"
#include <carl-arith/core/Variable.h>

#include <carl-arith/converter/LibpolyConverter.h>
#include <carl-logging/carl-logging.h>

#include <carl-arith/poly/lp/LPPolynomial.h>

#include "LPRan.h"

namespace carl {

RealRootsResult<LPRealAlgebraicNumber> real_roots(const LPPolynomial& polynomial, const Interval<LPRealAlgebraicNumber::NumberType>& interval = Interval<LPRealAlgebraicNumber::NumberType>::unbounded_interval());

RealRootsResult<LPRealAlgebraicNumber> real_roots(const LPPolynomial& polynomial, const std::map<Variable, LPRealAlgebraicNumber>& m, const Interval<LPRealAlgebraicNumber::NumberType>& interval = Interval<LPRealAlgebraicNumber::NumberType>::unbounded_interval());

}

#endif