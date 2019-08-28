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

/**
 * Compute a univariate polynomial with rational coefficients that has the roots of 'p' whose coefficient variables have been substituted by the roots given in m.
 * Note that the resulting polynomial has the main variable of p in the end.
 *
 * @param p
 * @param m
 * @return a univariate polynomial with rational coefficients (and p's main variable) that has the roots of p whose coefficient variables have been substituted by the roots given in m
 */
template<typename Number>
MultivariatePolynomial<Number> evaluatePolynomial(
		const MultivariatePolynomial<Number>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m
);

/**
 * Evaluate the coefficients of the given polynomial p w.r.t. the given evaluation map m.
 * The algorithm assumes that all variables in m are coefficient variables.
 *
 * @param p polynomial to be evaluated in the given variables. This should be a univariate polynomial in a variable <i>not</i> occurring in variables.
 * @param m map assigning each variable of p an interval-represented RealAlgebraicNumber
 * @return rational univariate polynomial having all real roots of the polynomial whose coefficients are evaluated according to m
 * @see Constraint::satisfiedBy and CAD::samples for usages of this method
 */
template<typename Number, typename Coeff>
UnivariatePolynomial<Number> evaluateCoefficients(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m
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

template<typename Number,  typename Coeff>
UnivariatePolynomial<Number> evaluatePolynomial(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m
) {
	CARL_LOG_TRACE("carl.ran", "EvaluatePolynomial " << p << " on " << m);
	UnivariatePolynomial<Coeff> pol(p);
	RANMap<Number> IRmap;
	
	for (const auto& r: m) {
		if (r.second.isNumeric()) {
			// Plug in numeric representations
			pol.substituteIn(r.first, Coeff(r.second.value()));
		} else {
			// Defer interval representations
			IRmap.emplace(r.first, r.second);
		}
	}
	if (pol.isNumber()) {
		return UnivariatePolynomial<Number>(p.mainVar(), pol.constantPart());
	}

	return overload_on_map<UnivariatePolynomial<Number>>(
		[&p](auto& map){ return UnivariatePolynomial<Number>(ran::evaluatePolynomial(p, map)); },
		IRmap
	);
}

}
}
