#pragma once

#include "../RealAlgebraicNumber.h"
#include "../RealAlgebraicNumberEvaluation.h"
#include <carl/interval/Interval.h>
#include <carl/core/logging.h>
#include <carl/core/Sign.h>
#include <carl/core/UnivariatePolynomial.h>

#include "RealRootIsolation.h"

#include <boost/optional.hpp>

#include <list>
#include <map>

#ifdef RAN_USE_Z3
#include "../z3/Z3RanRootFinder.h"
#endif

namespace carl {
namespace rootfinder {

////////////////////////////////////////
////////////////////////////////////////
// realRoots() for univariate polynomials

/**
 * Find all real roots of a univariate 'polynomial' with numeric coefficients within a given 'interval'.
 * The roots are sorted in ascending order.
 */
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type, EnableIf<std::is_same<Coeff, Number>> = dummy>
std::vector<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
) {
	CARL_LOG_DEBUG("carl.core.rootfinder", polynomial << " within " << interval);
	#ifdef RAN_USE_Z3
		auto r = realRootsZ3(polynomial, interval);
		CARL_LOG_DEBUG("carl.core.rootfinder", "-> " << r);
		return r;
	#else
		RealRootIsolation rri(polynomial, interval);
		auto r = rri.get_roots();
		CARL_LOG_DEBUG("carl.core.rootfinder", "-> " << r);
		return r;
	#endif
}

/**
 * Find all real roots of a univariate 'polynomial' with non-numeric coefficients within a given 'interval'.
 * However, all coefficients must be types that contain numeric numbers that are retrievable by using .constantPart();
 * The roots are sorted in ascending order.
 */
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type, DisableIf<std::is_same<Coeff, Number>> = dummy>
std::vector<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
) {
	assert(polynomial.isUnivariate());
	return realRoots(polynomial.convert(std::function<Number(const Coeff&)>([](const Coeff& c){ return c.constantPart(); })), interval);
}

////////////////////////////////////////
////////////////////////////////////////
// realRoots() for multivariate polynomials

/**
 * Replace all variables except one of the multivariate polynomial 'p' by
 * numbers as given in the mapping 'm', which creates a univariate polynomial,
 * and return all roots of that created polynomial.
 * Note that 'p' is represented as a univariate polynomial with polynomial coefficients.
 * Its main variable is not replaced and stays the main variable of the created polynomial.
 * However, all variables in the polynomial coefficients are replaced, which is why
 * <ul>
 *   <li>the main variable of 'p' must not be in 'm'</li>
 *   <li>all variables from the coefficients of 'p' must be in 'm'</li>
 * </ul>
 * The roots are sorted in ascending order.
 */
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
std::vector<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
);

template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
std::vector<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::list<Variable>& variables,
		const std::list<RealAlgebraicNumber<Number>>& values,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
);

/////////////////////////
// Auxiliary Functions //
/////////////////////////

/**
 * Count only the number of real roots of the 'polynomial' within the given 'interval'.
 * However, this function uses realRoots() to obtain the real roots.
 * TODO What's the point of this function? We can use realRoots(..).size().
 */
template<typename Coeff, typename Number= typename UnderlyingNumberType<Coeff>::type>
uint countRealRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
) {
	return realRoots(polynomial, interval).size();
}
}
}

#include "RootFinder.tpp"
