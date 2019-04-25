#pragma once

/**
 * @file
 * Collect functions to evaluate a polynomial wrt. algebraic reals, i.e.  plug
 * in algebraic reals for some or all of the variables inside a polynomial and
 * get the resulting polynomial or algebraic real.
 */

#include <map>
#include <vector>



#include "RealAlgebraicNumber.h"
#include "RealAlgebraicPoint.h"


#include "../../../core/MultivariatePolynomial.h"
#include "../../../core/polynomialfunctions/Resultant.h"
#include "../../../interval/IntervalEvaluation.h"
#include "../../../util/SFINAE.h"

namespace carl {

namespace RealAlgebraicNumberEvaluation {

template <typename Number>
using RANMap = std::map<Variable, RealAlgebraicNumber<Number>>;

namespace detail {
	template<typename Tag, typename F, typename Number>
	auto overload_on_map(Tag, F&& f, const RANMap<Number>& map) {
		std::map<Variable, Tag> tmp;
		for (const auto& m: map) {
			tmp.emplace(m.first, std::get<Tag>(m.second.content()));
		}
		return f(tmp);
	}
}

template<typename T, typename F, typename Number>
auto overload_on_map(F&& f, const RANMap<Number>& map) {
	assert(!map.empty());
	return std::visit(
		[&f, &map](const auto& tag){
			return T(detail::overload_on_map(tag, std::forward<F>(f), map));
		},
		map.begin()->second.content()
	);
}

/**
 * Evaluate the given polynomial 'p' at the given 'point' based on the variable order given by 'variables'.
 * If a variable is assigned a numeric representation, the corresponding value is directly plugged in.
 * All assignments of interval representations are passed on to <code>evaluate(MultivariatePolynomial, RANIRMap)</code>.
 * Note that the number of variables must match the dimension of the 'point', all
 * variables of 'p' must appear in 'variables' and that 'variables' must not mention any additional variables.
 */
template<typename Number, typename Coeff>
RealAlgebraicNumber<Number> evaluate(const MultivariatePolynomial<Coeff>& p, const RealAlgebraicPoint<Number>& point, const std::vector<Variable>& variables);

/**
 * Evaluate the given polynomial 'p' at the point represented by the variable-to-nummber-mapping 'm'.
 * If a variable is assigned a numeric representation, the corresponding value is directly plugged in.
 * All assignments of interval representations are passed on to <code>evaluate(MultivariatePolynomial, RANIRMap)</code>.
 * Note that variables of 'p' must be assigned in 'm' and that 'm' must not assign any additional variables.
 */
template<typename Number>
RealAlgebraicNumber<Number> evaluate(const MultivariatePolynomial<Number>& p, const RANMap<Number>& m);
template<typename Number>
RealAlgebraicNumber<Number> evaluateIR(const MultivariatePolynomial<Number>& p, const RANMap<Number>& m);

/**
 * Compute a univariate polynomial with rational coefficients that has the roots of 'p' whose coefficient variables have been substituted by the roots given in m.
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
 * Evaluate the coefficients of the given polynomial p w.r.t. the given evaluation map m.
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

	return overload_on_map<RealAlgebraicNumber<Number>>(
		[&pol](auto& map){ return RealAlgebraicNumber<Number>(ran::evaluate(pol, map)); },
		IRmap
	);
}

/**
 * Evaluate the given constraint 'c' at the given 'point' based on the variable order given by 'variables'.
 * If a variable is assigned a numeric representation, the corresponding value is directly plugged in.
 * All assignments of interval representations are passed on to <code>evaluate(Constraint, RANIRMap)</code>.
 * Note that the number of variables must match the dimension of the 'point', all
 * variables of 'c' must appear in 'variables' and that 'variables' must not mention any additional variables.
 */
template<typename Number, typename Poly>
bool evaluate(const Constraint<Poly>& c, const RANMap<Number>& m) {
	CARL_LOG_TRACE("carl.ran", "Evaluating " << c << " on " << m);
	MultivariatePolynomial<Number> pol(c.lhs());
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
		return evaluate(pol.constantPart(), c.relation());
	}

	// need to evaluate polynomial on non-trivial RANs

	Constraint<Poly> constr(pol, c.relation());

	return overload_on_map<bool>(
		[&constr](auto& map){ return bool(ran::evaluate(constr, map)); },
		IRmap
	);
}


/**
 * Evaluate the given polynomial with the given values for the variables.
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
		return RealAlgebraicNumber<Number>();
	}
	Variable v = freshRealVariable();
	// compute the result polynomial and the initial result interval
	std::map<Variable, Interval<Number>> varToInterval;
	UnivariatePolynomial<Number> res = evaluatePolynomial(UnivariatePolynomial<MultivariatePolynomial<Number>>(v, {MultivariatePolynomial<Number>(-p), MultivariatePolynomial<Number>(1)}), m, varToInterval);
	assert(!varToInterval.empty());
	poly = p.toUnivariatePolynomial(varToInterval.begin()->first);
	CARL_LOG_DEBUG("carl.ran", "res = " << res);
	CARL_LOG_DEBUG("carl.ran", "varToInterval = " << varToInterval);
	CARL_LOG_DEBUG("carl.ran", "poly = " << poly);
	Interval<Number> interval = IntervalEvaluation::evaluate(poly, varToInterval);
	CARL_LOG_DEBUG("carl.ran", "-> " << interval);

	auto sturmSeq = sturm_sequence(res);
	// the interval should include at least one root.
	assert(!carl::isZero(res));
	assert(
		res.sgn(interval.lower()) == Sign::ZERO ||
		res.sgn(interval.upper()) == Sign::ZERO ||
		count_real_roots(sturmSeq, interval) >= 1
	);
	while (
		res.sgn(interval.lower()) == Sign::ZERO ||
		res.sgn(interval.upper()) == Sign::ZERO ||
		count_real_roots(sturmSeq, interval) != 1) {
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
	CARL_LOG_DEBUG("carl.ran", "Result is " << RealAlgebraicNumber<Number>(res, interval, sturmSeq));
	return RealAlgebraicNumber<Number>(res, interval, sturmSeq);
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
		if (!tmp.has(i.first)) {
			if (p.has(i.first)) {
				CARL_LOG_DEBUG("carl.ran", i.first << " vanished from " << tmp << " but was present in " << p);
				if (i.second.isNumeric()) {
					varToInterval[i.first] = Interval<Number>(i.second.value(), BoundType::WEAK, i.second.value(), BoundType::WEAK);
				} else if (i.second.isInterval()) {
					// Variable vanished, add it to varToInterval
					varToInterval[i.first] = i.second.getInterval();
				} else {
					CARL_LOG_WARN("carl.ran", "Unknown type of RAN.");
				}
			}
			continue;
		}
		if (i.second.isNumeric()) {
			CARL_LOG_DEBUG("carl.ran", "Direct substitution: " << i.first << " = " << i.second);
			tmp.substituteIn(i.first, Coeff(i.second.value()));
			varToInterval[i.first] = Interval<Number>(i.second.value(), BoundType::WEAK, i.second.value(), BoundType::WEAK);
		} else if (i.second.isInterval()) {
			CARL_LOG_DEBUG("carl.ran", "IR substitution: " << i.first << " = " << i.second);
			i.second.simplifyByPolynomial(i.first, MultivariatePolynomial<Number>(tmp));
			UnivariatePolynomial<Coeff> p2(i.first, i.second.getIRPolynomial().template convert<Coeff>().coefficients());
			CARL_LOG_DEBUG("carl.ran", "Simplifying " << tmp.switchVariable(i.first) << " with " << p2);
			tmp = tmp.switchVariable(i.first).prem(p2);
			CARL_LOG_DEBUG("carl.ran", "Using " << p2 << " with " << tmp);
			tmp = carl::resultant(tmp, p2);
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

template<typename Number,  typename Coeff>
UnivariatePolynomial<Number> evaluatePolynomial(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m
) {
	CARL_LOG_DEBUG("carl.ran", "Evaluating " << p << " on " << m);
	Variable v = p.mainVar();
	UnivariatePolynomial<Coeff> tmp = p;
	for (const auto& i: m) {
		if (!tmp.has(i.first)) {
			// Variable vanished, skip it
			continue;
		}
		if (i.second.isNumeric()) {
			CARL_LOG_DEBUG("carl.ran", "Direct substitution: " << i.first << " = " << i.second);
			tmp.substituteIn(i.first, Coeff(i.second.value()));
		} else if (i.second.isInterval()) {
			CARL_LOG_DEBUG("carl.ran", "IR substitution: " << i.first << " = " << i.second);
			i.second.simplifyByPolynomial(i.first, MultivariatePolynomial<Number>(tmp));
			UnivariatePolynomial<Coeff> p2(i.first, i.second.getIRPolynomial().template convert<Coeff>().coefficients());
			CARL_LOG_DEBUG("carl.ran", "Using " << p2 << " with " << tmp.switchVariable(i.first));
			tmp = tmp.switchVariable(i.first).prem(p2);
			CARL_LOG_DEBUG("carl.ran", "Using " << p2 << " with " << tmp);
			tmp = carl::resultant(tmp, p2);
			CARL_LOG_DEBUG("carl.ran", "-> " << tmp);
		} else {
			CARL_LOG_WARN("carl.ran", "Unknown type of RAN.");
		}
		CARL_LOG_DEBUG("carl.ran", "Substituted " << i.first << " -> " << i.second << ", result: " << tmp);
	}
	CARL_LOG_DEBUG("carl.ran", "Result: " << MultivariatePolynomial<Number>(tmp));
	return tmp.switchVariable(v).toNumberCoefficients();
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
