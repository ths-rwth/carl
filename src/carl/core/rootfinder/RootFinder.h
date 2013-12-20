/* 
 * File:   RootFinder.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../RealAlgebraicNumber.h"
#include "../UnivariatePolynomial.h"
#include "../../interval/ExactInterval.h"
#include "../Sign.h"
#include "IncrementalRootFinder.h"

namespace carl {
namespace rootfinder {

//////////////////////
// realRoots() for univariate polynomials

/**
 * Finds all real roots of a univariate polynomial with numeric coefficients within a given interval.
 * @param polynomial
 * @param interval
 * @param pivoting
 * @return
 */
template<typename Coeff, typename Number, typename Finder = IncrementalRootFinder<Number>, EnableIf<std::is_same<Coeff, Number>> = dummy>
std::list<RealAlgebraicNumber<Number>*> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const ExactInterval<Number>& interval = ExactInterval<Number>::unboundedExactInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
) {
	return Finder(polynomial, interval, pivoting).getAllRoots();
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
std::list<RealAlgebraicNumber<Number>*> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		const ExactInterval<Number>& interval = ExactInterval<Number>::unboundedExactInterval(),
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
std::list<RealAlgebraicNumber<Number>*> realRoots(
		const UnivariatePolynomial<Coeff>& polynomial,
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT,
		const ExactInterval<Number>& interval = ExactInterval<Number>::unboundedExactInterval()
) {
	return realRoots(polynomial, interval, pivoting);
}

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
		const ExactInterval<Number>& interval = ExactInterval<Number>::unboundedExactInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
) {
	return realRoots(polynomial, interval, pivoting).size();
}

//////////////////////
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
std::list<RealAlgebraicNumber<Number>*> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>*>& m,
		const ExactInterval<Number>& interval = ExactInterval<Number>::unboundedExactInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
);

template<typename Coeff, typename Number = typename UnderlyingNumberType<Coeff>::type>
std::list<RealAlgebraicNumber<Number>*> realRoots(
		UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m,
		const ExactInterval<Number>& interval = ExactInterval<Number>::unboundedExactInterval(),
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT
);

/**
 * Computes the real roots of the univariate polynomial p, while its possibly polynomial coefficients are evaluated by the given variable-number lists represented as iterators.
 *
 * The result is a sorted list.
 *
 * @param p
 * @param pivoting strategy selection according to RealAlgebraicNumberSettings::IsolationStrategy (standard option is RealAlgebraicNumberSettings::DEFAULT_ISOLATIONSTRATEGY)
 * @param interval the initial interval for real-root isolation (default: zero interval)
 * @return the real roots of the univariate polynomial p where its coefficients are evaluated according to the given variable and value sequences
 */
template<class InputRIterator, class InputVIterator, typename Coeff, typename Number>
std::list<RealAlgebraicNumber<Number>*> realRoots(
		const UnivariatePolynomial<Coeff>& p,
		InputRIterator rBegin,
		InputRIterator rEnd,
		InputVIterator vBegin,
		InputVIterator vEnd,
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT,
		const ExactInterval<Number>& interval = ExactInterval<Number>::unboundedInterval()
);

/**
 * Isolates the real roots of the given univariate polynomial by evaluating its parameterized coefficients according to the given map.
 *
 * @param p possibly parameterized univariate polynomial
 * @param m evaluation map for the coefficients of p
 * @param pivoting strategy selection according to RealAlgebraicNumberSettings::IsolationStrategy (standard option is RealAlgebraicNumberSettings::DEFAULT_ISOLATIONSTRATEGY)
 * @param interval the initial interval for real-root isolation (default: zero interval)
 * @return sorted list containing the real roots of the given polynomial, which is evaluated according to the given map
 */
template<typename Number, typename Coeff, EnableIf<is_number<Number>> = dummy>
std::list<RealAlgebraicNumber<Number>*> realRootsEvalIR(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m,
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT,
		const ExactInterval<Number>& interval = ExactInterval<Number>::unboundedInterval()
);

/**
 * Isolates the real roots of the given univariate polynomial by evaluating its parameterized coefficients according to the given map.
 *
 * @param p possibly parameterized univariate polynomial
 * @param a vector with interval-represented RealAlgebraicNumbers
 * @param v the variables for evaluation corresponding to the real algebraic point
 * @param pivoting strategy selection according to RealAlgebraicNumberSettings::IsolationStrategy (standard option is RealAlgebraicNumberSettings::DEFAULT_ISOLATIONSTRATEGY)
 * @param interval the initial interval for real-root isolation (default: zero interval)
 * @return sorted list containing the real roots of the given polynomial, which is evaluated according to the given variables/numbers
 */
template<typename Number, typename Coeff, EnableIf<is_number<Number>> = dummy>
std::list<RealAlgebraicNumber<Number>*> realRootsEvalIR(
		const UnivariatePolynomial<Coeff>& p,
		const std::vector<RealAlgebraicNumberIR<Number>*>& a,
		const std::vector<Variable>& v,
		SplittingStrategy pivoting = SplittingStrategy::DEFAULT,
		const ExactInterval<Number>& interval = ExactInterval<Number>::unboundedInterval()
);

/**
 * Computes the list of common real roots of the given list of rational univariate polynomials.
 *
 * Note that the contents of the polynomials in l can be changed due to a normalization within the RealAlgebraicNumberIR constructor.
 *
 * @param l list of rational univariate polynomials
 * @return vector containing the common real roots of the given list of polynomials
 */
template<typename Number, EnableIf<is_number<Number>> = dummy>
std::list<RealAlgebraicNumber<Number>*> commonRealRoots(const std::list<UnivariatePolynomial<Number>>& l);

/**
 * Evaluates the coefficients of the given polynomial p w.r.t. the given evaluation map m.
 * The algorithm assumes that all variables in m are coefficient variables.
 *
 * The map varToInterval gives back an assignment of variables to the isolating intervals of the roots for each variable.
 * @param p polynomial to be evaluated in the given variables. This should be a univariate polynomial in a variable <i>not</i> occurring in variables.
 * @param m map assigning each variable of p an interval-represented RealAlgebraicNumber
 * @param varToInterval
 * @return rational univariate polynomial having all real roots of the polynomial whose coefficients are evaluated according to m
 * @see Constraint::satisfiedBy and CAD::samples for usages of this method
 */
template<typename Number, EnableIf<is_number<Number>> = dummy>
const UnivariatePolynomial<Number> evaluateCoefficientsIR(
		const UnivariatePolynomial<Number>& p,
		const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m,
		std::map<Variable, ExactInterval<Number>>& varToInterval
);

/**
 * Evaluates the given polynomial p in the given variables v at the given vector of RealAlgeraicNumberIR by substituting all variables.
 * a shall only contain RealAlgebraicNumberIRPtr instances.
 * <p>
 * The algorithm assumes that the variables in v are all variables occurring in p.
 * </p>
 * @param p polynomial to be evaluated in the given variables.
 * @param a vector with interval-represented RealAlgebraicNumbers
 * @param v the variables for evaluation corresponding to the real algebraic point
 * @return Real algebraic number representing the value of p evaluated at the point a
 * @see Constraint::satisfiedBy and CAD::samples for usages of this method
 */
template<typename Number, EnableIf<is_number<Number>> = dummy>
const RealAlgebraicNumber<Number>* evaluateIR(
		const UnivariatePolynomial<Number>& p,
		const std::vector<RealAlgebraicNumberIR<Number>*>& a,
		const std::vector<Variable>& v
);

/**
 * Evaluates the coefficients of the given polynomial p w.r.t. the given evaluation map m.
 * <p>
 * The algorithm assumes that the variables in m are all variables occurring in p.
 * </p>
 * @param p polynomial to be evaluated in the given variables. This should be a univariate polynomial in a variable <i>not</i> occurring in m
 * @param m map assigning each variable of p an interval-represented RealAlgebraicNumber
 * @return Real algebraic number representing the value of p evaluated according to m
 * @see Constraint::satisfiedBy and CAD::samples for usages of this method
 */
template<typename Number, EnableIf<is_number<Number>> = dummy>
const RealAlgebraicNumber<Number>* evaluateIR(const UnivariatePolynomial<Number>& p, const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m);

/////////////////////////
// Auxiliary Functions //
/////////////////////////

/** Helping method to find the real roots of a polynomial recursively.
 * This method offers several solving strategies available by setting the argument <code>pivoting</code>.
 * All have in common that 0 is returned as <code>RealAlgebraicNumberNR</code> if it happens to be a valid root. For details on the strategies @see
 * @param varMinLeft number of sign variations of seq at the minimal left endpoint
 * @param p pointer to polynomial whose roots are searched
 * @param seq standard Sturm of p
 * @param i isolating interval which shall be searched for real roots recursively
 * @param roots list of roots found so far
 * @param pivoting strategy selection according to RealAlgebraicNumberSettings::IsolationStrategy
 */
template<typename Number, EnableIf<is_number<Number>> = dummy>
void searchRealRoots(
		UnivariatePolynomial<Number>*& p,
		std::list<UnivariatePolynomial<Number>>& seq,
		const ExactInterval<Number>& i,
		unsigned variationsLeft,
		unsigned variationsRight,
		std::list<RealAlgebraicNumber<Number>*>* roots,
		SplittingStrategy pivoting
);

/**
 * Computes a univariate polynomial with rational coefficients that has the roots of p whose coefficient variables have been substituted by the roots given in m.
 * The map varToInterval gives back an assignment of variables to the isolating intervals of the roots for each variable.
 * Not that the resulting polynomial has the main variable of p in the end.
 *
 * @param p
 * @param m
 * @param varToInterval
 * @return a univariate polynomial with rational coefficients (and p's main variable) that has the roots of p whose coefficient variables have been substituted by the roots given in m
 */
template<typename Number, EnableIf<is_number<Number>> = dummy>
UnivariatePolynomial<Number> evaluatePolynomial(
		UnivariatePolynomial<Number>& p, 
		const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m,
		std::map<Variable, ExactInterval<Number>>& varToInterval
);

}
}

#include "RootFinder.tpp"