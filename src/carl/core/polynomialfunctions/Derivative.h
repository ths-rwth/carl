#pragma once

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

#include <algorithm>
#include <iterator>

namespace carl {
namespace detail_derivative {
	/// Returns n * (n-1) * ... * (n-k+1)
	constexpr inline std::size_t multiply(std::size_t n, std::size_t k) {
		std::size_t res = 1;
		for (; k > 0; --k) res *= n - k + 1;
		return res;
	}
}

/**
 * Computes the n'th derivative of a number, which is either the number itself (for n = 0) or zero.
 */
template<typename T, EnableIf<is_number<T>> = dummy>
const T& derivative(const T& t, Variable, std::size_t n = 1) {
	if (n == 0) return t;
	return constant_zero<T>::get();
}

/**
 * Computes the (partial) n'th derivative of this monomial with respect to the given variable.
 * @param m Monomial to derive.
 * @param v Variable.
 * @param n n.
 * @return Partial n'th derivative, consisting of constant factor and the remaining monomial.
 */
inline std::pair<std::size_t,Monomial::Arg> derivative(const Monomial::Arg& m, Variable v, std::size_t n = 1) {
	if (n == 0) return std::make_pair(1, m);
	auto it = std::find_if(m->begin(), m->end(),
		[v](const auto& e){ return e.first == v; }
	);
	if ((it == m->end()) || (it->second < n)) {
		CARL_LOG_DEBUG("carl.core", "derivative(" << m << ", " << v << ", " << n << ") = 0");
		return std::make_pair(0, nullptr);
	} else if (it->second == n) {
		std::size_t factor = detail_derivative::multiply(it->second, n);
		if (m->exponents().size() == 1) {
			CARL_LOG_DEBUG("carl.core", "derivative(" << m << ", " << v << ", " << n << ") = " << factor);
			return std::make_pair(factor, nullptr);
		} else {
			Monomial::Content newExps;
			for (const auto& e: *m) {
				if (e.first == v) continue;
				newExps.emplace_back(e);
			}
			auto res = createMonomial(std::move(newExps), m->tdeg()-n);
			CARL_LOG_DEBUG("carl.core", "derivative(" << m << ", " << v << ", " << n << ") = " << factor << "*" << res);
			return std::make_pair(factor, res);
		}
	} else {
		std::size_t factor = detail_derivative::multiply(it->second, n);
		Monomial::Content newExps = m->exponents();
		newExps[static_cast<std::size_t>(std::distance(m->begin(), it))].second -= n;
		auto res = createMonomial(std::move(newExps), m->tdeg()-n);
		CARL_LOG_DEBUG("carl.core", "derivative(" << m << ", " << v << ", " << n << ") = " << factor << "*" << res);
		return std::make_pair(factor, res);
	}
}

/**
 * Computes the n'th derivative of t with respect to v.
 */
template<typename C>
Term<C> derivative(const Term<C>& t, Variable v, std::size_t n = 1) {
	if (n == 0) return t;
	if (t.monomial()) {
		auto newm = carl::derivative(t.monomial(), v, n);
		return Term<C>(t.coeff() * newm.first, newm.second);
	} else {
		return Term<C>();
	}
}

/**
 * Computes the n'th derivative of p with respect to v.
 */
template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> derivative(const MultivariatePolynomial<C,O,P>& p, Variable v, std::size_t n = 1)
{
	// Check for trivial cases.
	if (n == 0) return p;
	if (p.isConstant()) return MultivariatePolynomial<C,O,P>();

	typename MultivariatePolynomial<C,O,P>::TermsType newTerms;
	for (const auto& t: p) {
		newTerms.emplace_back(derivative(t, v, n));
		if (isZero(newTerms.back())) newTerms.pop_back();
	}
	return MultivariatePolynomial<C,O,P>(std::move(newTerms));
}

/**
 * Computes the n'th derivative of p with respect to the main variable of p.
 */
template<typename C>
UnivariatePolynomial<C> derivative(const UnivariatePolynomial<C>& p, std::size_t n = 1) {
	if (n == 0) return p;
	if (isZero(p)) return p;
	if (p.degree() < n) {
		CARL_LOG_DEBUG("carl.core", "derivative(" << p << ", " << n << ") = 0");
		return UnivariatePolynomial<C>(p.mainVar());
	}

	std::vector<C> newCoeffs;
	for (std::size_t i = 0; i < p.degree() + 1 - n; ++i) {
		std::size_t factor = detail_derivative::multiply(n + i, n);
		newCoeffs.emplace_back(static_cast<C>(factor) * p.coefficients()[i + n]);
	}
	CARL_LOG_DEBUG("carl.core", "derivative(" << p << ", " << n << ") = " << UnivariatePolynomial<C>(p.mainVar(), newCoeffs));
	return UnivariatePolynomial<C>(p.mainVar(), std::move(newCoeffs));
}

/**
 * Computes the n'th derivative of p with respect to v.
 */
template<typename C>
UnivariatePolynomial<C> derivative(const UnivariatePolynomial<C>& p, Variable v, std::size_t n = 1) {
	if (n == 0) return p;
	if (isZero(p)) return p;
	if (v == p.mainVar()) return derivative(p, n);

	std::vector<C> newCoeffs;
	std::transform(p.coefficients().begin(), p.coefficients().end(), std::back_inserter(newCoeffs),
		[v,n](const auto& c) { return derivative(c, v, n); }
	);
	CARL_LOG_DEBUG("carl.core", "derivative(" << p << ", " << n << ") = " << UnivariatePolynomial<C>(p.mainVar(), newCoeffs));
	return UnivariatePolynomial<C>(p.mainVar(), std::move(newCoeffs));
}


}