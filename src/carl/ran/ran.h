#pragma once

/**
 * @file
 * Represent a real algebraic number (RAN) in one of several ways:
 * - Implicitly by a univariate polynomial and an interval.
 * - Implicitly by a polynomial and a sequence of signs (called Thom encoding).
 * - Explicitly by a rational number.
 * Rationale:
 * - A real number cannot always be adequately represented in finite memory, since
 *   it may be infinitely long. Representing
 *   it by a float or any other finite-precision representation and doing
 *   arithmatic may introduce unacceptable rouding errors.
 *   The algebraic reals, a subset of the reals, is the set of those reals that can be represented
 *   as the root of a univariate polynomial with rational coefficients so there is always
 *   an implicit, finite, full-precision
 *   representation by an univariate polynomial and an isolating interval that
 *   contains this root (only this root and no other). It is also possible
 *   to do relatively fast arithmetic with this representation without rounding errors.
 * - When the algebraic real is only finitely long prefer the rational number
 *   representation because it's faster.
 * - The idea of the Thom-Encoding is as follows: Take a square-free univariate polynomial p
 *   with degree n that has the algebraic real as its root, compute the first n-1 derivates of p,
 *   plug in this algebraic real into each derivate and only keep the sign.
 *   Then polynomial p with this sequence of signs uniquely represents this algebraic real.
 */


#include <carl/config.h>

#ifdef RAN_USE_INTERVAL
#include "interval/ran_interval.h"
#include "interval/ran_interval_evaluation.h"
#endif

#ifdef RAN_USE_THOM
#include "thom/ran_thom.h"
#include "thom/ran_thom_evaluation.h"
#endif

#ifdef RAN_USE_Z3
#include "z3/ran_z3.h"
#include "z3/ran_z3_evaluation.h"
#endif

#ifdef RAN_USE_LIBPOLY
#include "libpoly/ran_libpoly.h"
#include "libpoly/ran_libpoly_evaluation.h"
#endif

namespace carl {
	#ifdef RAN_USE_INTERVAL
	template<typename Number>
	using real_algebraic_number = real_algebraic_number_interval<Number>;
	#endif

	#ifdef RAN_USE_THOM
	template<typename Number>
	using real_algebraic_number = real_algebraic_number_thom<Number>;
	#endif

	#ifdef RAN_USE_Z3
	template<typename Number>
	using real_algebraic_number = real_algebraic_number_z3<Number>;
	#endif

	#ifdef RAN_USE_LIBPOLY
	template<typename Number>
	using real_algebraic_number = real_algebraic_number_libpoly<Number>;
	#endif

	template<typename Number>
	using ran_assignment = Assignment<real_algebraic_number<Number>>;

	template<typename Number>
	using ordered_ran_assignment = OrderedAssignment<real_algebraic_number<Number>>;


	template<typename Number>
	using RealAlgebraicNumber = real_algebraic_number<Number>;

	namespace ran {
		template<typename Number>
		using RANMap = ran_assignment<Number>;
	}
}