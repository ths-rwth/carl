#pragma once

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"
#include <carl-arith/core/Variables.h>

namespace carl {

/**
 * Convert a univariate polynomial that is currently (mis)represented by a
 * 'MultivariatePolynomial' into a more appropiate 'UnivariatePolynomial'
 * representation.
 * Note that the current polynomial must mention one and only one variable,
 * i.e., be indeed univariate.
 */
template<typename C, typename O, typename P>
UnivariatePolynomial<C> to_univariate_polynomial(const MultivariatePolynomial<C,O,P>& p) {
	// Only correct when it is already only in one variable.
	assert(variables(p).size() == 1);
	Variable::Arg x = p.lmon()->single_variable();
	std::vector<C> coeffs(p.total_degree()+1,0);
	for (const auto& t : p)
	{
		coeffs[t.tdeg()] = t.coeff();
	}
	return UnivariatePolynomial<C>(x, coeffs);
}

/**
 * Convert a multivariate polynomial that is currently represented by a
 * MultivariatePolynomial into a UnivariatePolynomial representation.
 * The main variable of the resulting polynomial is given as second argument.
 */
template<typename C, typename O, typename P>
UnivariatePolynomial<MultivariatePolynomial<C,O,P>> to_univariate_polynomial(const MultivariatePolynomial<C,O,P>& p, Variable v) {
	assert(p.is_consistent());
	std::vector<MultivariatePolynomial<C,O,P>> coeffs(1);
	for (const auto& term: p) {
		if (term.monomial() == nullptr) coeffs[0] += term;
		else {
			const auto& mon = term.monomial();
			auto exponent = mon->exponent_of_variable(v);
			if (exponent >= coeffs.size()) {
				coeffs.resize(exponent + 1);
			}
			std::shared_ptr<const carl::Monomial> tmp = mon->drop_variable(v);
			coeffs[exponent] += term.coeff() * tmp;
		}
	}
	// Convert result back to MultivariatePolynomial and check that the result is equal to p
	assert(MultivariatePolynomial<C>(UnivariatePolynomial<MultivariatePolynomial<C>>(v, coeffs)) == p);
	return UnivariatePolynomial<MultivariatePolynomial<C,O,P>>(v, coeffs);
}

}