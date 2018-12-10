#pragma once

#include "../../formula/model/ran/RealAlgebraicNumber.h"
#include "../../formula/model/ran/RealAlgebraicNumberEvaluation.h"
#include "../../interval/Interval.h"
#include "../logging.h"
#include "../Sign.h"
#include "../UnivariatePolynomial.h"
#include "IncrementalRootFinder.h"

#include <boost/optional.hpp>

#include <list>
#include <map>

#include "../../formula/model/ran/adaption_z3/Z3RanRootFinder.h"

namespace carl {
namespace rootfinder {

////////////////////////////////////////
////////////////////////////////////////
// realRoots() for univariate polynomials

/**
 * Find all real roots of a univariate 'polynomial' with numeric coefficients within a given 'interval'.
 */
template<typename Coeff, typename Number, typename Finder = IncrementalRootFinder<Number>, EnableIf<std::is_same<Coeff, Number>> = dummy>
std::vector<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
) {
	CARL_LOG_DEBUG("carl.core.rootfinder", polynomial << " within " << interval);
	#ifdef USE_Z3_RANS
	auto r = realRootsZ3(polynomial, interval);
	#else
	auto r = Finder(polynomial, interval, pivoting).getAllRoots();
	#endif
	CARL_LOG_DEBUG("carl.core.rootfinder", "-> " << r);
	return r;
}

/**
 * Find all real roots of a univariate 'polynomial' with non-numeric coefficients within a given 'interval'.
 * However, all coefficients must be types that contain numeric numbers that are retrievable by using .constantPart();
 */
template<typename Coeff, typename Number, DisableIf<std::is_same<Coeff, Number>> = dummy>
std::vector<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
) {
	assert(polynomial.isUnivariate());
	return realRoots(polynomial.convert(std::function<Number(const Coeff&)>([](const Coeff& c){ return c.constantPart(); })), interval, pivoting);
}


/**
 * Find all real roots of a univariate 'polynomial' with non-numeric coefficients within a given 'interval'.
 * However, all coefficients must be types that contain numeric numbers that are retrievable by using .constantPart();
 * Note that this is a convenience method with swapped arguments to omit the interval but give a strategy.
 */
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
std::vector<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
) {
	return realRoots(polynomial, interval, pivoting);
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
 */
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
std::vector<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
);

template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
std::vector<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::list<Variable>& variables,
		const std::list<RealAlgebraicNumber<Number>>& values,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
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
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
) {
	return realRoots(polynomial, interval, pivoting).size();
}
}
}

#include "RootFinder.tpp"
