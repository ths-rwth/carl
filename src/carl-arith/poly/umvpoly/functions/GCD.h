#pragma once

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"
#include <carl-arith/numbers/typetraits.h>
#include "VarInfo.h"

namespace carl {
	template<typename C, typename O, typename P>
	MultivariatePolynomial<C,O,P> gcd(const MultivariatePolynomial<C,O,P>& a, const MultivariatePolynomial<C,O,P>& b);
	
	template<typename Coeff>
	UnivariatePolynomial<Coeff> gcd(const UnivariatePolynomial<Coeff>& a, const UnivariatePolynomial<Coeff>& b);
}

#include "GCD_multivariate.h"
#include "GCD_univariate.h"
#include "PrimitiveEuclidean.h"

namespace carl {

template<typename C, typename O, typename P>
Term<C> gcd(const MultivariatePolynomial<C,O,P>& a, const Term<C>& b) {
	static_assert(is_field_type<C>::value, "Only implemented for field coefficients");
	assert(!is_zero(a));
	assert(!is_zero(b));
	if (b.is_constant()) return Term<C>(C(1));
	return Term<C>(C(1), gcd(a, b.monomial()));
}

template<typename C, typename O, typename P>
Term<C> gcd(const Term<C>& a, const MultivariatePolynomial<C,O,P>& b) {
	return gcd(b, a);
}

template<typename C, typename O, typename P>
Monomial::Arg gcd(const MultivariatePolynomial<C,O,P>& a, const Monomial::Arg& b) {
	if (!b) return nullptr;
	assert(!is_zero(a));
	auto varinfo = carl::vars_info(a,false);
	std::vector<std::pair<Variable, exponent>> vepairs;
	for (const auto& ve : *b) {
		if (varinfo.var(ve.first)->num_occurences() == a.nr_terms()) {
			vepairs.push_back(ve.first, std::min(varinfo.var(ve.first)->min_degree(), ve.second));
		}
	}
	return createMonomial(std::move(vepairs));
}

template<typename C, typename O, typename P>
Monomial::Arg gcd(const Monomial::Arg& a, const MultivariatePolynomial<C,O,P>& b) {
	return gcd(b, a);
}

}