/**
 * @file RootFinder.h
 * @ingroup rootfinder
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <list>
#include <map>

#include "../../formula/model/ran/RealAlgebraicNumber.h"
#include "../../formula/model/ran/RealAlgebraicNumberEvaluation.h"
#include "../../interval/Interval.h"
#include "../logging.h"
#include "../Sign.h"
#include "../UnivariatePolynomial.h"
#include "IncrementalRootFinder.h"

namespace carl {
namespace rootfinder {

////////////////////////////////////////
////////////////////////////////////////
// realRoots() for univariate polynomials

/**
 * Finds all real roots of a univariate polynomial with numeric coefficients within a given interval.
 * @param polynomial
 * @param interval
 * @param pivoting
 * @return
 */
template<typename Coeff, typename Number, typename Finder = IncrementalRootFinder<Number>, EnableIf<std::is_same<Coeff, Number>> = dummy>
std::list<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
) {
	CARL_LOG_FUNC("carl.core.rootfinder", polynomial << ", " << interval);
	auto r = Finder(polynomial, interval, pivoting).getAllRoots();
	CARL_LOG_TRACE("carl.core.rootfinder", "-> " << r);
	return r;
}

/**
 * Finds all real roots of a univariate polynomial with non-numeric coefficients within a given interval.
 * This method asserts that all these coefficients are however numbers wrapped in another type and tries to retrieve the numbers using .constantPart();
 * @param polynomial
 * @param interval
 * @param pivoting
 * @return
 */
template<typename Coeff, typename Number, DisableIf<std::is_same<Coeff, Number>> = dummy>
std::list<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
) {
	assert(polynomial.isUnivariate());
	return realRoots(polynomial.convert(std::function<Number(const Coeff&)>([](const Coeff& c){ return c.constantPart(); })), interval, pivoting);
}

/**
 * Convenience method so we can swap the arguments and omit the interval but give a strategy.
 * Calls realRoots with the given univariate polynomial.
 * @param polynomial
 * @param pivoting
 * @param interval
 * @return
 */
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
std::list<RealAlgebraicNumber<Number>> realRoots(
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
 * Finds all real roots of the polynomial p within the given interval.
 *
 * This methods substitutes all variables of the coefficients of p as given in the map.
 * It asserts that
 * <ul>
 *   <li>the main variable of p is not in m</li>
 *   <li>all variables from the coefficients of p are in m</li>
 * </ul>
 * @param p
 * @param m
 * @param interval
 * @param pivoting
 * @return
 */
template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
std::list<RealAlgebraicNumber<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
);

template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
std::list<RealAlgebraicNumber<Number>> realRoots(
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
 * Calculates the number of real roots of the given polynomial within the given interval.
 * Uses realRoots() to obtain the real roots.
 * @param polynomial
 * @param interval
 * @param pivoting
 * @return
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
