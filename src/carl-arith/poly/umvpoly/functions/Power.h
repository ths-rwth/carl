#pragma once

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

namespace carl {

/**
 * Calculates the given power of a monomial m.
 * @param m The monomial.
 * @param exp Exponent.
 * @return m to the power of exp.
 */
inline Monomial::Arg pow(const Monomial& m, uint exp) {
	if (exp == 0) {
		return nullptr;
	}
	Monomial::Content newExps;
	uint expsum = 0;
	for (const auto& it: m.exponents()) {
		newExps.emplace_back(it.first, uint(it.second * exp));
		expsum += newExps.back().second;
	}
	return createMonomial(std::move(newExps), expsum);
}
inline Monomial::Arg pow(const Monomial::Arg& m, uint exp) {
	if (m == nullptr) return nullptr;
	return pow(*m, exp);
}

template<typename Coeff>
Term<Coeff> pow(const Term<Coeff>& t, uint exp) {
	if (t.monomial()) {
		return Term<Coeff>(carl::pow(t.coeff(), exp), pow(*t.monomial(), exp));
	} else {
		return Term<Coeff>(carl::pow(t.coeff(), exp), nullptr);
	}
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> pow(const MultivariatePolynomial<C,O,P>& p, std::size_t exp) {
	//std::cout << "pw(" << *this << " ^ " << exp << ")" << std::endl;
	if (carl::is_zero(p)) return MultivariatePolynomial<C,O,P>(constant_zero<C>::get());
	if (exp == 0) return MultivariatePolynomial<C,O,P>(constant_one<C>::get());
	if (exp == 1) return MultivariatePolynomial<C,O,P>(p);
	if (exp == 2) return p*p;
	MultivariatePolynomial<C,O,P> res(constant_one<C>::get());
	MultivariatePolynomial<C,O,P> mult(p);
	while(exp > 0) {
		res *= mult;
		exp--;
	}
	return res;
}

template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> pow_naive(const MultivariatePolynomial<C,O,P>& p, std::size_t exp) {
	if (exp == 0) {
		return MultivariatePolynomial(constant_one<C>::get());
	}

	CARL_LOG_INEFFICIENT();
	MultivariatePolynomial<C,O,P> res(p);
	for (std::size_t i = 1; i < exp; i++) {
		res *= p;
	}
	return res;
}

/**
 * Returns a polynomial to the given power.
 * @param p The polynomial.
 * @param exp Exponent.
 * @return The polynomial to the power of exp.
 */
template<typename Coeff>
UnivariatePolynomial<Coeff> pow(const UnivariatePolynomial<Coeff>& p, std::size_t exp) {
	if (exp == 0) return UnivariatePolynomial<Coeff>(p.mainVar(), constant_one<Coeff>::get());
	if (carl::is_zero(p)) return UnivariatePolynomial<Coeff>(p.mainVar());
	UnivariatePolynomial<Coeff> res(p.mainVar(), constant_one<Coeff>::get());
	UnivariatePolynomial<Coeff> mult(p);
	while (exp > 0) {
		if ((exp & 1) != 0) res *= mult;
		exp /= 2;
		if(exp > 0) mult = mult * mult;
	}
	return res;
}

}