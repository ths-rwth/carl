#pragma once

namespace carl {
	template<typename C, typename O, typename P>
	class MultivariatePolynomial;
	template<typename C, typename O, typename P>
	MultivariatePolynomial<C,O,P> gcd(const MultivariatePolynomial<C,O,P>& a, const MultivariatePolynomial<C,O,P>& b);
	
	template<typename Coeff>
	class UnivariatePolynomial;
	template<typename Coeff>
	UnivariatePolynomial<Coeff> gcd(const UnivariatePolynomial<Coeff>& a, const UnivariatePolynomial<Coeff>& b);
}

#include "GCD_multivariate.h"
#include "GCD_univariate.h"
#include "PrimitiveEuclidean.h"
#include "../MultivariatePolynomial.h"
#include "../../numbers/typetraits.h"

namespace carl {

template<typename C, typename O, typename P>
Term<C> gcd(const MultivariatePolynomial<C,O,P>& a, const Term<C>& b) {
	static_assert(is_field<C>::value, "Only implemented for field coefficients");
	assert(!a.isZero());
	assert(!b.isZero());
	if (b.isConstant()) return Term<C>(C(1));
	return Term<C>(C(1), gcd(a, b.monomial()));
}

template<typename C, typename O, typename P>
Term<C> gcd(const Term<C>& a, const MultivariatePolynomial<C,O,P>& b) {
	return gcd(b, a);
}

template<typename C, typename O, typename P>
Monomial::Arg gcd(const MultivariatePolynomial<C,O,P>& a, const Monomial::Arg& b) {
	if (!b) return nullptr;
	assert(!a.isZero());
	VariablesInformation<false, MultivariatePolynomial<C,O,P>> varinfo = a.getVarInfo();
	std::vector<std::pair<Variable, exponent>> vepairs;
	for (const auto& ve : *b) {
		if (varinfo.getVarInfo(ve.first)->occurence() == a.nrTerms()) {
			vepairs.push_back(ve.first, std::min(varinfo.getVarInfo(ve.first)->minDegree(), ve.second));
		}
	}
	return createMonomial(std::move(vepairs));
}

template<typename C, typename O, typename P>
Monomial::Arg gcd(const Monomial::Arg& a, const MultivariatePolynomial<C,O,P>& b) {
	return gcd(b, a);
}

}