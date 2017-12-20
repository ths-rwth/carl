#pragma once

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
	if (p.isConstant()) return carl::constant_zero<Coeff>::get();
	
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
	if (p.isConstant()) return carl::constant_zero<Coeff>::get();
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
	if (p.isConstant()) return carl::constant_zero<Coeff>::get();
	
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


}
