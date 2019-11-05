#pragma once

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"
#include "../Variables.h"

namespace carl {

template<typename C, typename O, typename P>
UnivariatePolynomial<C> to_univariate_polynomial(const MultivariatePolynomial<C,O,P>& p) {
	// Only correct when it is already only in one variable.
	assert(variables(p).size() == 1);
	Variable::Arg x = p.lmon()->getSingleVariable();
	std::vector<C> coeffs(p.totalDegree()+1,0);
	for (const auto& t : p)
	{
		coeffs[t.tdeg()] = t.coeff();
	}
	return UnivariatePolynomial<C>(x, coeffs);
}

template<typename C, typename O, typename P>
UnivariatePolynomial<MultivariatePolynomial<C,O,P>> to_univariate_polynomial(const MultivariatePolynomial<C,O,P>& p, Variable v) {
	assert(p.isConsistent());
	std::vector<MultivariatePolynomial<C,O,P>> coeffs(1);
	for (const auto& term: p) {
		if (term.monomial() == nullptr) coeffs[0] += term;
		else {
			const auto& mon = term.monomial();
			auto exponent = mon->exponentOfVariable(v);
			if (exponent >= coeffs.size()) {
				coeffs.resize(exponent + 1);
			}
			std::shared_ptr<const carl::Monomial> tmp = mon->dropVariable(v);
			coeffs[exponent] += term.coeff() * tmp;
		}
	}
	// Convert result back to MultivariatePolynomial and check that the result is equal to p
	assert(MultivariatePolynomial<C>(UnivariatePolynomial<MultivariatePolynomial<C,O,P>>(v, coeffs)) == p);
	return UnivariatePolynomial<MultivariatePolynomial<C,O,P>>(v, coeffs);
}

}