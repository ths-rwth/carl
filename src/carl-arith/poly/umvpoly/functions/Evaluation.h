#pragma once

#include "Degree.h"

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace carl {

template<typename Coefficient>
Coefficient evaluate(const Monomial& m, const std::map<Variable, Coefficient>& substitutions) {
	CARL_LOG_FUNC("carl.core.monomial", m << ", " << substitutions);
	Coefficient res = carl::constant_one<Coefficient>::get();
	for (const auto& ve : m) {
		auto it = substitutions.find(ve.first);
		assert(it != substitutions.end());
		res *= carl::pow(it->second, ve.second);
	}
	CARL_LOG_TRACE("carl.core.monomial", "Result: " << res);
	return res;
}

template<typename Coefficient>
Coefficient evaluate(const Term<Coefficient>& t, const std::map<Variable, Coefficient>& map) {
	if (t.monomial()) {
		return t.coeff() * evaluate(*t.monomial(), map);
	} else {
		return t.coeff();
	}
}

/**
 * Like substitute, but expects substitutions for all variables.
 * @return For a polynomial p, the function value p(x_1,...,x_n).
 */
template<typename C, typename O, typename P, typename SubstitutionType>
SubstitutionType evaluate(const MultivariatePolynomial<C,O,P>& p, const std::map<Variable, SubstitutionType>& substitutions) {
	if(carl::is_zero(p)) {
		return constant_zero<SubstitutionType>::get();
	} else {
		SubstitutionType result(evaluate(p[0], substitutions)); 
		for (unsigned i = 1; i < p.nr_terms(); ++i) {
			result += evaluate(p[i], substitutions);
		}
		return result;
	};
}

template<typename Coeff>
Coeff evaluate(const UnivariatePolynomial<Coeff>& p, const Coeff& value) {
	Coeff result(0);
	Coeff var(1);
	for (const Coeff& coeff : p.coefficients()) {
		result += (coeff * var);
		var *= value;
	}
	return result;
}


template<typename Coeff>
bool is_root_of(const UnivariatePolynomial<Coeff>& p, const Coeff& value) {
	return carl::sgn(carl::evaluate(p, value)) == Sign::ZERO;
}

}
