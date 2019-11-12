#pragma once

#include "Degree.h"

#include "../UnivariatePolynomial.h"

#include <algorithm>
#include <numeric>

namespace carl {

/*
 * Computes an upper bounds on the absolute value of the real roots of the given univariate polynomial due to Cauchy.
 *
 * The bound is defined as $\f 1 + \frac{\max \{ |a_0|, ..., |a_{n-1}| \}}{|a_n|} $\f where $a_n \neq 0$ is the leading coefficient.
 * If $p = 0$, we return $0$.
 */
template<typename Coeff>
Coeff cauchyBound(const UnivariatePolynomial<Coeff>& p) {
	static_assert(is_field<Coeff>::value, "Cauchy bounds are only defined for field-coefficients");
	if (is_constant(p)) return carl::constant_zero<Coeff>::get();
	
	auto it = std::max_element(p.coefficients().begin(), std::prev(p.coefficients().end()), 
		[](const auto& a, const auto& b){ return carl::abs(a) < carl::abs(b); }
	);
	return 1 + carl::abs(*it) / carl::abs(p.lcoeff());
}

/*
 * Computes an upper bounds on the absolute value of the real roots of the given univariate polynomial due to Hirst and Macey @cite Hirst1997.
 *
 * The bound is defined as $\f max\{ 1, (|a_0| + ... + |a_{n-1}|)/|a_n| \} $\f where $a_n \neq 0$ is the leading coefficient.
 */
template<typename Coeff>
Coeff hirstMaceyBound(const UnivariatePolynomial<Coeff>& p) {
	static_assert(is_field<Coeff>::value, "HirstMacey bounds are only defined for field-coefficients");
	if (is_constant(p)) return carl::constant_zero<Coeff>::get();
	auto sum = std::accumulate(
		p.coefficients().begin(),
		std::prev(p.coefficients().end()),
		carl::constant_zero<Coeff>::get(),
		[](const auto& a, const auto& b){ 
			return static_cast<Coeff>(a + carl::abs(b));
		}
	);
	return std::max(carl::constant_one<Coeff>::get(), sum / carl::abs(p.lcoeff()));
}

/*
 * Computes an upper bounds on the absolute value of the real roots of the given univariate polynomial due to Lagrange @cite Mignotte2002.
 *
 * The bound is defined as $\f 2 \cdot max\{ \frac{|a_0|}{|a_n|}^{1/n}, ..., \frac{|a_{n-1}|}{|a_n|}^{1} \} $\f where $a_n \neq 0$ is the leading coefficient.
 */
template<typename Coeff>
Coeff lagrangeBound(const UnivariatePolynomial<Coeff>& p) {
	static_assert(is_field<Coeff>::value, "Lagrange bounds are only defined for field-coefficients");
	if (is_constant(p)) return carl::constant_zero<Coeff>::get();
	
	Coeff max;
	Coeff lc = carl::abs(p.lcoeff());
	for (std::size_t i = 1; i <= p.degree(); ++i) {
		if (carl::isZero(p.coefficients()[p.degree()-i])) continue;
		auto cur = carl::abs(p.coefficients()[p.degree()-i]) / lc;
		if (i > 1) {
			cur = carl::root_safe(cur, i).second;
		}
		max = std::max(max, cur);
	}
	return 2 * max;
}

/*
 * Computes an upper bound on the value of the positive real roots of the given univariate polynomial due to Lagrange.
 * 
 * See https://en.wikipedia.org/wiki/Geometrical_properties_of_polynomial_roots#Other_bounds
 *
 * The bound is defined as $\f 2 * \max \{ \frac{-a_{n-k}}{a_n}^{1/k} \mid 1 \leq k \leq n, a_k < 0 \} $\f where $a_n > 0$ is the leading coefficient.
 * If a_n < 0, the coefficients are multiplied by -1 as this does not change the roots. Accordingly, it is then defined as
 * $\f 2 * \max \{ \frac{-a_{n-k}}{a_n}^{1/k} \mid 1 \leq k \leq n, a_k > 0 \} $\f where $a_n < 0$ is the leading coefficient.
 * If the set in $\max$ is empty, then there are no positive real roots and we return $0$.
 * If $p$ is constant, we return $0$.
 */
template<typename Coeff>
Coeff lagrangePositiveUpperBound(const UnivariatePolynomial<Coeff>& p) {

	static_assert(is_field<Coeff>::value, "Lagrange bounds are only defined for field-coefficients");
	if (is_constant(p)) return carl::constant_zero<Coeff>::get();

	Coeff max;
	Coeff lc = p.lcoeff();

	for (std::size_t i = 1; i <= p.degree(); ++i) {
		if (carl::isZero(p.coefficients()[p.degree()-i])) continue;
		if (carl::isPositive(lc * p.coefficients()[p.degree()-i])) continue;

		auto cur = carl::abs(p.coefficients()[p.degree()-i]) / carl::abs(lc);
		if (i > 1) {
			cur = carl::root_safe(cur, i).second;
		}
		max = std::max(max, cur);
	}
	return 2 * max;
}

/**
 * Computes a lower bound on the value of the positive real roots of the given univariate polynomial.
 * 
 * Let \f$Q(x) = x^q*P(1/x)\f$. Then \f$P(1/a) = 0 \rightarrow Q(a) = 0\f$. Thus \f$a <= b \forall a\f$ with \f$Q(a) = 0 \rightarrow 1/b <= a' \forall a'\f$ with \f$P(a') = 0\f$.
 * Note that the coefficients of Q are the ones of P in reverse order.
 */
template<typename Coeff>
Coeff lagrangePositiveLowerBound(const UnivariatePolynomial<Coeff>& p) {
	auto r = lagrangePositiveUpperBound(p.reverseCoefficients());
	if (r == 0) return 0;
	return 1/r;
}

/**
 * Computes an upper bound on the value of the negative real roots of the given univariate polynomial.
 * 
 * Note that the positive roots of P(-x) are the negative roots of P(x).
 * 
 */
template<typename Coeff>
Coeff lagrangeNegativeUpperBound(const UnivariatePolynomial<Coeff>& p) {
	return -lagrangePositiveLowerBound(p.negateVariable());
}

}
