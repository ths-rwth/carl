#pragma once

#include <carl-common/config.h>

#ifdef USE_LIBPOLY

#include "../real_roots_common.h"
#include <carl-arith/core/Variable.h>

#include <carl-arith/converter/LibpolyConverter.h>
#include <carl-logging/carl-logging.h>

#include <carl-arith/poly/lp/LPPolynomial.h>

#include "LPRan.h"

namespace carl::ran::libpoly {

RealRootsResult<RealAlgebraicNumberLibpoly> real_roots_libpoly(const LPPolynomial& polynomial, const Interval<RealAlgebraicNumberLibpoly::NumberType>& interval = Interval<RealAlgebraicNumberLibpoly::NumberType>::unbounded_interval());

RealRootsResult<RealAlgebraicNumberLibpoly> real_roots_libpoly(const LPPolynomial& polynomial, const std::map<Variable, RealAlgebraicNumberLibpoly>& m, const Interval<RealAlgebraicNumberLibpoly::NumberType>& interval = Interval<RealAlgebraicNumberLibpoly::NumberType>::unbounded_interval());

} // namespace carl::ran::libpoly

#endif