#pragma once
#include <carl-arith/interval/Interval.h>
#include <carl-arith/interval/Power.h>

#include "../Monomial.h"
#include "../Term.h"
#include "../MultivariatePolynomial.h"

namespace carl {

template<typename Numeric>
inline Interval<Numeric> evaluate(const Monomial& m, const std::map<Variable, Interval<Numeric>>& map)
{
	Interval<Numeric> result(1);
	CARL_LOG_TRACE("carl.core.intervalevaluation", "Iterating over " << m);
	for(unsigned i = 0; i < m.num_variables(); ++i)
	{
		CARL_LOG_TRACE("carl.core.intervalevaluation", "Iterating: " << m[i].first);
		// We expect every variable to be in the map.
		CARL_LOG_ASSERT("carl.core.intervalevaluation", map.count(m[i].first) > (size_t)0, "Every variable is expected to be in the map.");
		result *= carl::pow(map.at(m[i].first), m[i].second);
        if( result.is_zero() )
            return result;
	}
	return result;
}

template<typename Coeff, typename Numeric, EnableIf<std::is_same<Numeric, Coeff>> = dummy>
inline Interval<Numeric> evaluate(const Term<Coeff>& t, const std::map<Variable, Interval<Numeric>>& map)
{
	Interval<Numeric> result(t.coeff());
	if (t.monomial())
		result *= evaluate( *t.monomial(), map );
	return result;
}

template<typename Coeff, typename Numeric, DisableIf<std::is_same<Numeric, Coeff>> = dummy>
inline Interval<Numeric> evaluate(const Term<Coeff>& t, const std::map<Variable, Interval<Numeric>>& map)
{
	Interval<Numeric> result(t.coeff());
	if (t.monomial())
		result *= evaluate( *t.monomial(), map );
	return result;
}

template<typename Coeff, typename Policy, typename Ordering, typename Numeric>
inline Interval<Numeric> evaluate(const MultivariatePolynomial<Coeff, Policy, Ordering>& p, const std::map<Variable, Interval<Numeric>>& map)
{
	CARL_LOG_FUNC("carl.core.intervalevaluation", p << ", " << map);
	if(is_zero(p)) {
		return Interval<Numeric>(0);
	} else {
		Interval<Numeric> result(evaluate(p[0], map)); 
		for (unsigned i = 1; i < p.nr_terms(); ++i) {
            if( result.is_infinite() )
                return result;
			result += evaluate(p[i], map);
		}
		return result;
	}
}

template<typename Numeric, typename Coeff, EnableIf<std::is_same<Numeric, Coeff>> = dummy>
inline Interval<Numeric> evaluate(const UnivariatePolynomial<Coeff>& p, const std::map<Variable, Interval<Numeric>>& map) {
	CARL_LOG_FUNC("carl.core.intervalevaluation", p << ", " << map);
	assert(map.count(p.main_var()) > 0);
	Interval<Numeric> res = Interval<Numeric>::empty_interval();
	const Interval<Numeric> varValue = map.at(p.main_var());
	Interval<Numeric> exp(1);
	for (unsigned i = 0; i < p.degree(); i++) {
		res += p.coefficients()[i] * exp;
        if( res.is_infinite() )
            return res;
		exp = varValue.pow(i+1);
	}
	return res;
}

template<typename Numeric, typename Coeff, DisableIf<std::is_same<Numeric, Coeff>> = dummy>
inline Interval<Numeric> evaluate(const UnivariatePolynomial<Coeff>& p, const std::map<Variable, Interval<Numeric>>& map) {
	CARL_LOG_FUNC("carl.core.intervalevaluation", p << ", " << map);
	assert(map.count(p.main_var()) > 0);
	Interval<Numeric> res = Interval<Numeric>(carl::constant_zero<Numeric>().get());
	const Interval<Numeric>& varValue = map.at(p.main_var());
	Interval<Numeric> exp(1);
	for (uint i = 0; i <= p.degree(); i++) {
		res += evaluate(p.coefficients()[i], map) * exp;
        if( res.is_infinite() )
            return res;
		exp = carl::pow(varValue, i+1);
	}
	return res;
}

} //Namespace carl
