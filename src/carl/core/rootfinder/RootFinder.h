/**
 * @file RootFinder.h
 * @ingroup rootfinder
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <list>
#include <map>

#include "../RealAlgebraicNumber.h"
#include "../RealAlgebraicNumberEvaluation.h"
#include "../UnivariatePolynomial.h"
#include "../../interval/Interval.h"
#include "../Sign.h"
#include "../logging.h"
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
std::list<RealAlgebraicNumberPtr<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
) {
	LOG_FUNC("carl.core.rootfinder", polynomial << ", " << interval);
	auto r = Finder(polynomial, interval, pivoting).getAllRoots();
	LOGMSG_TRACE("carl.core.rootfinder", "RealRoots of " << polynomial << " in " << interval << ":" << std::endl << r);
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
std::list<RealAlgebraicNumberPtr<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
) {
	LOG_FUNC("carl.core.rootfinder", polynomial << ", " << interval);
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
std::list<RealAlgebraicNumberPtr<Number>> realRoots(
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
std::list<RealAlgebraicNumberPtr<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumberPtr<Number>>& m,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
);

template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
std::list<RealAlgebraicNumberPtr<Number>> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::list<Variable> variables,
		const std::list<RealAlgebraicNumberPtr<Number>> values,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
);

template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
std::list<RealAlgebraicNumberPtr<Number>> realRoots(
		UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumberIRPtr<Number>>& m,
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
unsigned long countRealRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const Interval<Number>& interval = Interval<Number>::unboundedInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
) {
	return realRoots(polynomial, interval, pivoting).size();
}
}
}

#include "RootFinder.tpp"
