/** 
 * @file   IntervalEvaluation.h
 *         Created on August 23, 2013, 1:12 PM
 * @author: Sebastian Junges
 * @author: Stefan Schupp
 * 
 */


#pragma once

#include "DoubleInterval.h"
#include "Interval.h"

#include "../core/Monomial.h"
#include "../core/Term.h"
#include "../core/MultivariatePolynomial.h"

namespace carl
{
class IntervalEvaluation
{
public:
	template<typename Numeric>
	static Interval<Numeric> evaluate(const Monomial& m, const std::map<Variable, Interval<Numeric>>&);

	template<typename Coeff, typename Numeric, EnableIf<std::is_same<Numeric, Coeff>> = dummy>
	static Interval<Numeric> evaluate(const Term<Coeff>& t, const std::map<Variable, Interval<Numeric>>&);
	template<typename Coeff, typename Numeric, DisableIf<std::is_same<Numeric, Coeff>> = dummy>
	static Interval<Numeric> evaluate(const Term<Coeff>& t, const std::map<Variable, Interval<Numeric>>&);

	template<typename Coeff, typename Policy, typename Ordering, typename Numeric>
	static Interval<Numeric> evaluate(const MultivariatePolynomial<Coeff, Policy, Ordering>& p, const std::map<Variable, Interval<Numeric>>&);

	template<typename Numeric, typename Coeff, EnableIf<std::is_same<Numeric, Coeff>> = dummy>
	static Interval<Numeric> evaluate(const UnivariatePolynomial<Coeff>& p, const std::map<Variable, Interval<Numeric>>& map);
	template<typename Numeric, typename Coeff, DisableIf<std::is_same<Numeric, Coeff>> = dummy>
	static Interval<Numeric> evaluate(const UnivariatePolynomial<Coeff>& p, const std::map<Variable, Interval<Numeric>>& map);
    
private:

};


template<typename Numeric>
inline Interval<Numeric> IntervalEvaluation::evaluate(const Monomial& m, const std::map<Variable, Interval<Numeric>>& map)
{
	Interval<Numeric> result(1);
	// TODO use iterator.
	LOGMSG_TRACE("carl.core.monomial", "Iterating over " << m);
	for(unsigned i = 0; i < m.nrVariables(); ++i)
	{
		LOGMSG_TRACE("carl.core.monomial", "Iterating: " << m[i].first);
		// We expect every variable to be in the map.
		LOG_ASSERT("carl.interval", map.count(m[i].first) > (size_t)0, "Every variable is expected to be in the map.");
		result *= map.at(m[i].first).power(m[i].second);
	}
	return result;
}

template<typename Coeff, typename Numeric, EnableIf<std::is_same<Numeric, Coeff>>>
inline Interval<Numeric> IntervalEvaluation::evaluate(const Term<Coeff>& t, const std::map<Variable, Interval<Numeric>>& map)
{
	Interval<Numeric> result(t.coeff());
	if (t.monomial()) {
		const Monomial& m = *t.monomial();
		// TODO use iterator.
		LOGMSG_TRACE("carl.core.monomial", "Iterating over " << m);
		for (unsigned i = 0; i < m.nrVariables(); ++i) {
			LOGMSG_TRACE("carl.core.monomial", "Iterating: " << m[i].first);
			// We expect every variable to be in the map.
			assert(map.count(m[i].first) > 0);
			result *= map.at(m[i].first).power(m[i].second);
		}
	}
	return result;
}

template<typename Coeff, typename Numeric, DisableIf<std::is_same<Numeric, Coeff>>>
inline Interval<Numeric> IntervalEvaluation::evaluate(const Term<Coeff>& t, const std::map<Variable, Interval<Numeric>>& map)
{
	Interval<Numeric> result(carl::toDouble(t.coeff()));
	if (t.monomial()) {
		const Monomial& m = *t.monomial();
		// TODO use iterator.
		LOGMSG_TRACE("carl.core.monomial", "Iterating over " << m);
		for (unsigned i = 0; i < m.nrVariables(); ++i) {
			LOGMSG_TRACE("carl.core.monomial", "Iterating: " << m[i].first);
			// We expect every variable to be in the map.
			assert(map.count(m[i].first) > 0);
			result *= map.at(m[i].first).power(m[i].second);
		}
	}
	return result;
}

template<typename Coeff, typename Policy, typename Ordering, typename Numeric>
inline Interval<Numeric> IntervalEvaluation::evaluate(const MultivariatePolynomial<Coeff, Policy, Ordering>& p, const std::map<Variable, Interval<Numeric>>& map)
{
	LOG_FUNC("carl.core.monomial", p << ", " << map);
	if(p.isZero()) {
		return Interval<Numeric>(0);
	} else {
		Interval<Numeric> result(evaluate(*p[0], map)); 
		for (unsigned i = 1; i < p.nrTerms(); ++i) {
			result += evaluate(*p[i], map);
		}
		return result;
	}
}

template<typename Numeric, typename Coeff, EnableIf<std::is_same<Numeric, Coeff>>>
inline Interval<Numeric> IntervalEvaluation::evaluate(const UnivariatePolynomial<Coeff>& p, const std::map<Variable, Interval<Numeric>>& map) {
	LOG_FUNC("carl.core.monomial", p << ", " << map);
	assert(map.count(p.mainVar()) > 0);
	Interval<Numeric> res = Interval<Numeric>::emptyInterval();
	const Interval<Numeric> varValue = map.at(p.mainVar());
	Interval<Numeric> exp(1);
	for (unsigned i = 0; i < p.degree(); i++) {
		res += p.coefficients()[i] * exp;
		exp *= varValue;
	}
	return res;
}

template<typename Numeric, typename Coeff, DisableIf<std::is_same<Numeric, Coeff>>>
inline Interval<Numeric> IntervalEvaluation::evaluate(const UnivariatePolynomial<Coeff>& p, const std::map<Variable, Interval<Numeric>>& map) {
	LOG_FUNC("carl.core.monomial", p << ", " << map);
	assert(map.count(p.mainVar()) > 0);
	Interval<Numeric> res = Interval<Numeric>::emptyInterval();
	const Interval<Numeric> varValue = map.at(p.mainVar());
	Interval<Numeric> exp(1);
	for (unsigned i = 0; i <= p.degree(); i++) {
		res += IntervalEvaluation::evaluate(p.coefficients()[i], map) * exp;
		exp *= varValue;
	}
	return res;
}

}


