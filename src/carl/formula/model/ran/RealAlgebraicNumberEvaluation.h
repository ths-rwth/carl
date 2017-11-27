/**
 * @file RealAlgebraicNumberEvaluation.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <map>
#include <vector>



#include "RealAlgebraicNumber.h"
#include "RealAlgebraicPoint.h"

#include "../../../core/MultivariatePolynomial.h"
#include "../../../interval/IntervalEvaluation.h"
#include "../../../thom/ThomEvaluation.h"   
#include "../../../util/SFINAE.h"

namespace carl {
namespace RealAlgebraicNumberEvaluation {

template <typename Number>
using RANMap = std::map<Variable, RealAlgebraicNumber<Number>>;

/**
 * Evaluates the given polynomial at the given point based on the variable order.
 * Asserts that the number of variables matches the dimension of the point, all variables of p have an assignment in m and that m has no additional assignments.
 * If a variable is assigned a numeric representation, the corresponding value is directly plugged in.
 * All assignments of interval representations are passed on to <code>evaluate(MultivariatePolynomial, RANIRMap)</code>.
 * 
 * @param p Polynomial to be evaluated
 * @param point Values for variables
 * @param variables Variables to be assigned
 * @return Evaluation result
 */
template<typename Number, typename Coeff>
RealAlgebraicNumber<Number> evaluate(const MultivariatePolynomial<Coeff>& p, const RealAlgebraicPoint<Number>& point, const std::vector<Variable>& variables);

/**
 * Evaluates the given polynomial with the given values for the variables.
 * Asserts that all variables of p have an assignment in m and that m has no additional assignments.
 * If a variable is assigned a numeric representation, the corresponding value is directly plugged in.
 * All assignments of interval representations are passed on to <code>evaluate(MultivariatePolynomial, RANIRMap)</code>.
 * 
 * @param p Polynomial to be evaluated
 * @param m Variable assignment
 * @return Evaluation result
 */
template<typename Number>
RealAlgebraicNumber<Number> evaluate(const MultivariatePolynomial<Number>& p, const RANMap<Number>& m);
template<typename Number>
RealAlgebraicNumber<Number> evaluateIR(const MultivariatePolynomial<Number>& p, const RANMap<Number>& m);

/**
 * Computes a univariate polynomial with rational coefficients that has the roots of p whose coefficient variables have been substituted by the roots given in m.
 * The map varToInterval gives back an assignment of variables to the isolating intervals of the roots for each variable.
 * Note that the resulting polynomial has the main variable of p in the end.
 *
 * @param p
 * @param m
 * @param varToInterval
 * @return a univariate polynomial with rational coefficients (and p's main variable) that has the roots of p whose coefficient variables have been substituted by the roots given in m
 */
template<typename Number, typename Coeff>
UnivariatePolynomial<Number> evaluatePolynomial(
		const UnivariatePolynomial<Coeff>& p, 
		const std::map<Variable, RealAlgebraicNumber<Number>>& m,
		std::map<Variable, Interval<Number>>& varToInterval
);
template<typename Number>
MultivariatePolynomial<Number> evaluatePolynomial(
		const MultivariatePolynomial<Number>& p, 
		const std::map<Variable, RealAlgebraicNumber<Number>>& m
);

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
template<typename Number, typename Coeff>
UnivariatePolynomial<Number> evaluateCoefficients(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m,
		std::map<Variable, Interval<Number>>& varToInterval
);


////////////////////////////////////////
////////////////////////////////////////
// Implementation

// This is called by carl::CAD implementation (from Constraint)
template<typename Number, typename Coeff>
RealAlgebraicNumber<Number> evaluate(const MultivariatePolynomial<Coeff>& p, const RealAlgebraicPoint<Number>& point, const std::vector<Variable>& variables) {
        assert(point.dim() == variables.size());
	RANMap<Number> RANs;
	MultivariatePolynomial<Coeff> pol(p);
	for (std::size_t i = 0; i < point.dim(); i++) {
		if (!pol.has(variables[i])) continue;
		assert(pol.has(variables[i]));
		if (point[i].isNumeric()) {
			// Plug in numeric representations
			pol.substituteIn(variables[i], MultivariatePolynomial<Coeff>(point[i].value()));
		} else {
			// Defer interval representations
			RANs.emplace(variables[i], point[i]);
		}
	}
	if (pol.isNumber()) {
		return RealAlgebraicNumber<Number>(pol.constantPart());
	}
	return evaluate(pol, RANs);
}

// This is called by smtrat::CAD implementation (from CAD.h)
template<typename Number>
RealAlgebraicNumber<Number> evaluate(const MultivariatePolynomial<Number>& p, const RANMap<Number>& m) {
	CARL_LOG_TRACE("carl.ran", "Evaluating " << p << " on " << m);
	MultivariatePolynomial<Number> pol(p);
	RANMap<Number> IRmap;
	
	for (const auto& r: m) {
		//assert(pol.has(it->first));
		if (r.second.isNumeric()) {
			// Plug in numeric representations
			pol.substituteIn(r.first, MultivariatePolynomial<Number>(r.second.value()));
		} else {
			// Defer interval representations
			IRmap.emplace(r.first, r.second);
		}
	}
	if (pol.isNumber()) {
		return RealAlgebraicNumber<Number>(pol.constantPart());
	}

	// need to evaluate polynomial on non-trivial RANs
	assert(IRmap.size() > 0);
	if(IRmap.begin()->second.isInterval()) {
		return evaluateIR(pol, IRmap);
	} else {
		return evaluateTE(pol, IRmap);
	}
}


/**
 * Evaluates the given polynomial with the given values for the variables.
 * Asserts that all variables of p have an assignment in m and that m has no additional assignments.
 * 
 * @param p Polynomial to be evaluated
 * @param m Variable assignment
 * @return Evaluation result
 */
template<typename Number>
RealAlgebraicNumber<Number> evaluateIR(const MultivariatePolynomial<Number>& p, const RANMap<Number>& m) {
	CARL_LOG_DEBUG("carl.ran", "Evaluating " << p << " on " << m);
	assert(m.size() > 0);
	auto poly = p.toUnivariatePolynomial(m.begin()->first);
	if (m.size() == 1 && m.begin()->second.sgn(poly.toNumberCoefficients()) == Sign::ZERO) {
		return RealAlgebraicNumber<Number>(poly.mainVar());
	}
	Variable v = freshRealVariable();
	// compute the result polynomial and the initial result interval
	std::map<Variable, Interval<Number>> varToInterval;
	UnivariatePolynomial<Number> res = evaluatePolynomial(UnivariatePolynomial<MultivariatePolynomial<Number>>(v, {MultivariatePolynomial<Number>(-p), MultivariatePolynomial<Number>(1)}), m, varToInterval);
	CARL_LOG_DEBUG("carl.ran", "res = " << res);
	Interval<Number> interval = IntervalEvaluation::evaluate(poly, varToInterval);

	// the interval should include at least one root.
	assert(!res.isZero());
	assert(
		res.sgn(interval.lower()) == Sign::ZERO ||
		res.sgn(interval.upper()) == Sign::ZERO ||
		res.countRealRoots(interval) >= 1
	);
	while (
		res.sgn(interval.lower()) == Sign::ZERO ||
		res.sgn(interval.upper()) == Sign::ZERO ||
		res.countRealRoots(interval) != 1) {
		// refine the result interval until it isolates exactly one real root of the result polynomial
		for (auto it = m.begin(); it != m.end(); it++) {
			it->second.refine();
			if (it->second.isNumeric()) {
				return evaluate(p, m);
			} else if (it->second.isInterval()) {
				varToInterval[it->first] = it->second.getInterval();
			} else {
				CARL_LOG_WARN("carl.ran", "Unknown type of RAN.");
			}
		}
		interval = IntervalEvaluation::evaluate(poly, varToInterval);
	}
	CARL_LOG_DEBUG("carl.ran", "Result is " << RealAlgebraicNumber<Number>(res, interval));
	return RealAlgebraicNumber<Number>(res, interval);
}


template<typename Number, typename Coeff>
UnivariatePolynomial<Number> evaluatePolynomial(
		const UnivariatePolynomial<Coeff>& p, 
		const std::map<Variable, RealAlgebraicNumber<Number>>& m,
		std::map<Variable, Interval<Number>>& varToInterval
) {
	CARL_LOG_DEBUG("carl.ran", "Evaluating " << p << " on " << m);
	Variable v = p.mainVar();
	UnivariatePolynomial<Coeff> tmp = p;
	for (const auto& i: m) {
		if (i.second.isNumeric()) {
			CARL_LOG_DEBUG("carl.ran", "Direct substitution: " << i.first << " = " << i.second);
			tmp.substituteIn(i.first, Coeff(i.second.value()));
		} else if (i.second.isInterval()) {
			CARL_LOG_DEBUG("carl.ran", "IR substitution: " << i.first << " = " << i.second);
			i.second.simplifyByPolynomial(i.first, MultivariatePolynomial<Number>(tmp));
			UnivariatePolynomial<Coeff> p2(i.first, i.second.getIRPolynomial().template convert<Coeff>().coefficients());
			CARL_LOG_DEBUG("carl.ran", "Using " << p2 << " with " << tmp.switchVariable(i.first));
			tmp = tmp.switchVariable(i.first).resultant(p2);
			CARL_LOG_DEBUG("carl.ran", "-> " << tmp);
			varToInterval[i.first] = i.second.getInterval();
		} else {
			CARL_LOG_WARN("carl.ran", "Unknown type of RAN.");
		}
		CARL_LOG_DEBUG("carl.ran", "Substituted " << i.first << " -> " << i.second << ", result: " << tmp);
	}
	CARL_LOG_DEBUG("carl.ran", "Result: " << tmp.switchVariable(v).toNumberCoefficients());
	return tmp.switchVariable(v).toNumberCoefficients();
}

template<typename Number>
MultivariatePolynomial<Number> evaluatePolynomial(
		const MultivariatePolynomial<Number>& p, 
		const std::map<Variable, RealAlgebraicNumber<Number>>& m
) {
	CARL_LOG_DEBUG("carl.ran", "Evaluating " << p << " on " << m);
	using Coeff = MultivariatePolynomial<Number>;
	UnivariatePolynomial<Coeff> tmp = p.toUnivariatePolynomial(m.begin()->first);
	for (const auto& i: m) {
		if (i.second.isNumeric()) {
			tmp.substituteIn(i.first, Coeff(i.second.value()));
		} else if (i.second.isInterval()) {
			UnivariatePolynomial<Coeff> p2(i.first, i.second.getPolynomial().template convert<Coeff>().coefficients());
			tmp = tmp.switchVariable(i.first).resultant(p2);
		} else {
			CARL_LOG_WARN("carl.ran", "Unknown type of RAN.");
		}
		CARL_LOG_DEBUG("carl.ran", "Substituted " << i.first << " -> " << i.second << ", result: " << tmp);
	}
	CARL_LOG_DEBUG("carl.ran", "Result: " << MultivariatePolynomial<Number>(tmp));
	return MultivariatePolynomial<Number>(tmp);
}


template<typename Number, typename Coeff>
UnivariatePolynomial<Number> evaluateCoefficients(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m,
		std::map<Variable, Interval<Number>>& varToInterval
) {
	CARL_LOG_DEBUG("carl.ran", "Evaluating " << p << " on " << m);
	assert(m.find(p.mainVar()) == m.end());
	return evaluatePolynomial(p, m, varToInterval);
}

}
}
