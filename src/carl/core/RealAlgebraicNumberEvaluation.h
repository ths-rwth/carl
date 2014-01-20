/* 
 * File:   RealAlgebraicNumberEvaluation.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <map>
#include <vector>

#include "../util/SFINAE.h"

#include "../interval/IntervalEvaluation.h"
#include "UnivariatePolynomial.h"
#include "RealAlgebraicNumber.h"
#include "RealAlgebraicPoint.h"

namespace carl {
namespace RealAlgebraicNumberEvaluation {

template <typename Number>
using RANMap = std::map<Variable, RealAlgebraicNumber<Number>*>;
template <typename Number>
using RANIRMap = std::map<Variable, RealAlgebraicNumberIR<Number>*>;

/**
 * Evaluates the given polynomial at the given point based on the variable order.
 * Asserts that the number of variables matches the dimension of the point, all variables of p have an assignment in m and that m has no additional assignments.
 * If a variable is assigned a numeric representation, the corresponding value is directly plugged in.
 * All assignments of interval representations are passed on to <code>evaluate(UnivariatePolynomial, RANIRMap)</code>.
 * 
 * @param p Polynomial to be evaluated
 * @param point Values for variables
 * @param variables Variables to be assigned
 * @return Evaluation result
 */
template<typename Number, typename Coeff>
RealAlgebraicNumber<Number>* evaluate(const UnivariatePolynomial<Coeff>& p, RealAlgebraicPoint<Number>& point, const std::vector<Variable>& variables);

/**
 * Evaluates the given polynomial with the given values for the variables.
 * Asserts that all variables of p have an assignment in m and that m has no additional assignments.
 * If a variable is assigned a numeric representation, the corresponding value is directly plugged in.
 * All assignments of interval representations are passed on to <code>evaluate(UnivariatePolynomial, RANIRMap)</code>.
 * 
 * @param p Polynomial to be evaluated
 * @param m Variable assignment
 * @return Evaluation result
 */
template<typename Number, typename Coeff>
RealAlgebraicNumber<Number>* evaluate(const UnivariatePolynomial<Coeff>& p, const RANMap<Number>& m);
template<typename Number, typename Coeff>
RealAlgebraicNumber<Number>* evaluate(const UnivariatePolynomial<Coeff>& p, const RANIRMap<Number>& m);


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
		const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m,
		std::map<Variable, ExactInterval<Number>>& varToInterval
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
		UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m,
		std::map<Variable, ExactInterval<Number>>& varToInterval
);


////////////////////////////////////////
////////////////////////////////////////
// Implementation

template<typename Number, typename Coeff>
RealAlgebraicNumber<Number>* evaluate(const UnivariatePolynomial<Coeff>& p, RealAlgebraicPoint<Number>& point, const std::vector<Variable>& variables) {
	assert(point.dim() == variables.size());
	RANIRMap<Number> IRs;
	UnivariatePolynomial<Coeff> pol(p);
	
	for (unsigned i = 0; i < point.dim(); i++) {
		assert(pol.has(variables[i]));
		if (point[i]->isNumeric()) {
			// Plug in numeric representations
			pol.substituteIn(variables[i], Coeff(point[i]->value()));
		} else {
			// Defer interval representations
			IRs[variables[i]] = static_cast<RealAlgebraicNumberIR<Number>*>(point[i]);
		}
	}
	LOGMSG_TRACE("carl.core", p << " at " << point << " = " << pol);
	if (pol.isNumber()) {
		return new RealAlgebraicNumberNR<Number>(pol.constantPart());
	}
	return evaluate(pol, IRs);
}

template<typename Number, typename Coeff>
RealAlgebraicNumber<Number>* evaluate(const UnivariatePolynomial<Coeff>& p, RANMap<Number>& m) {
	RANIRMap<Number> IRs;
	UnivariatePolynomial<Coeff> pol(p);
	
	for (auto it: m) {
		assert(pol.has(it.first));
		if (it.second->isNumeric()) {
			// Plug in numeric representations
			pol.substituteIn(it.first, Coeff(it.second->value()));
		} else {
			// Defer interval representations
			IRs[it.first] = static_cast<const RealAlgebraicNumberIR<Number>*>(it.second);
		}
	}
	if (pol.isNumber()) {
		return new RealAlgebraicNumberNR<Number>(pol.numericContent());
	}
	return evaluate(pol, IRs);
}

/**
 * Evaluates the given polynomial with the given values for the variables.
 * Asserts that all variables of p have an assignment in m and that m has no additional assignments.
 * 
 * @param p Polynomial to be evaluated
 * @param m Variable assignment
 * @return Evaluation result
 */
template<typename Number, typename Coeff>
RealAlgebraicNumber<Number>* evaluate(const UnivariatePolynomial<Coeff>& p, const RANIRMap<Number>& m) {
	if (m.size() == 1 && m.begin()->second->sgn(p.toNumberCoefficients()) == Sign::ZERO) {
		return new RealAlgebraicNumberIR<Number>(p.mainVar());
	}
	Variable v = VariablePool::getInstance().getFreshVariable();
	// compute the result polynomial and the initial result interval
	std::map<Variable, ExactInterval<Number>> varToInterval;
	UnivariatePolynomial<Number> res = evaluatePolynomial(UnivariatePolynomial<Coeff>(v, {-Coeff(p), Coeff(1)}), m, varToInterval);
	ExactInterval<Number> interval = IntervalEvaluation::evaluate(p, varToInterval);
	
	while (
		res.sgn(interval.left()) == Sign::ZERO ||
		res.sgn(interval.right()) == Sign::ZERO ||
		res.countRealRoots(interval) != 1) {
		// refine the result interval until it isolates exactly one real root of the result polynomial
		for (auto it: m) {
			it.second->refine();
			varToInterval[it.first] = it.second->getInterval();
		}
		interval = IntervalEvaluation::evaluate(p, varToInterval);
	}
	return new RealAlgebraicNumberIR<Number>(res, interval);
}


template<typename Number, typename Coeff>
UnivariatePolynomial<Number> evaluatePolynomial(
		const UnivariatePolynomial<Coeff>& p, 
		const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m,
		std::map<Variable, ExactInterval<Number>>& varToInterval
) {
	Variable v = p.mainVar();
	UnivariatePolynomial<Coeff> tmp = p;
	for (auto i: m) {
		UnivariatePolynomial<Coeff> p2(i.first, i.second->getPolynomial().template convert<Coeff>().coefficients());
		tmp = tmp.switchVariable(i.first).resultant(p2);
		varToInterval[i.first] = i.second->getInterval();
	}
	return tmp.switchVariable(v).toNumberCoefficients();
}


template<typename Number, typename Coeff>
UnivariatePolynomial<Number> evaluateCoefficients(
		UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumberIR<Number>*>& m,
		std::map<Variable, ExactInterval<Number>>& varToInterval
) {
	assert(m.find(p.mainVar()) == m.end());
	return evaluatePolynomial(p, m, varToInterval);
}

}
}

