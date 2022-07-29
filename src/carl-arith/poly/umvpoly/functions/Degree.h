#pragma once

#include "../MultivariatePolynomial.h"
#include "../UnivariatePolynomial.h"

/**
 * @file Degree.h
 * Implements utility functions concerning the (total) degree of monomials, terms and polynomials.
 * - total_degree()
 * - is_zero()
 * - is_one()
 * - is_constant()
 * - is_linear()
 * - is_at_most_linear()
 * 
 */

namespace carl {
		
/**
 * Gives the total degree, i.e. the sum of all exponents.
 * @return Total degree.
 */
inline auto total_degree(const Monomial& m) {
	return m.tdeg();
}

/**
 * Checks whether the monomial is a constant.
 * @return If monomial is constant.
 */
inline bool is_constant(const Monomial& m) {
	return total_degree(m) == 0;
}

/**
 * Checks whether the monomial has exactly degree one.
 * @return If monomial is linear.
 */
inline bool is_linear(const Monomial& m) {
	return total_degree(m) == 1;
}

/**
 * Checks whether the monomial has at most degree one.
 * @return If monomial is linear or constant.
 */
inline bool is_at_most_linear(const Monomial& m) {
	return total_degree(m) <= 1;
}

/**
 * Gives the total degree, i.e. the sum of all exponents.
 * @return Total degree.
 */
template<typename Coeff>
std::size_t total_degree(const Term<Coeff>& t) {
	if (!t.monomial()) return 0;
	return total_degree(*t.monomial());
}

// /**
//  * Checks whether a term is zero.
//  */
// template<typename Coeff>
// bool is_zero(const Term<Coeff>& term) {
// 	return carl::is_zero(term.coeff());
// }
// 
// /**
//  * Checks whether a term is one.
//  */
// template<typename Coeff>
// bool is_one(const Term<Coeff>& term) {
// 	return is_constant(term) && carl::is_one(term.coeff());
// }

/**
 * Checks whether the monomial is a constant.
 * @return 
 */
template<typename Coeff>
bool is_constant(const Term<Coeff>& t) {
	return !t.monomial();
}

/**
 * Checks whether the monomial has exactly the degree one.
 * @return 
 */
template<typename Coeff>
bool is_linear(const Term<Coeff>& t) {
	if (!t.monomial()) return false;
	return is_linear(*t.monomial());
}

/**
 * Checks whether the monomial has at most degree one.
 * @return If monomial is linear or constant.
 */
template<typename Coeff>
bool is_at_most_linear(const Term<Coeff>& t) {
	if (!t.monomial()) return true;
	return is_at_most_linear(*t.monomial());
}

/**
 * Calculates the max. degree over all monomials occurring in the polynomial.
 * As the degree of the zero polynomial is \f$-\infty\f$, we assert that this polynomial is not zero. This must be checked by the caller before calling this method.
 * @see @cite GCL92, page 48
 * @return Total degree.
 */
template<typename Coeff, typename Ordering, typename Policies>
std::size_t total_degree(const MultivariatePolynomial<Coeff,Ordering,Policies>& p) {
	if (carl::is_zero(p)) return 0;
	assert(p.size() != 0);
	if (Ordering::degreeOrder) {
		return total_degree(p.lterm());
	} else {
		CARL_LOG_NOTIMPLEMENTED();
	}
}

// template<typename C, typename O, typename P>
// bool is_one(const MultivariatePolynomial<C,O,P>& p) {
// 	return (p.nr_terms() == 1) && is_one(p.lterm());
// }
// 
// template<typename C, typename O, typename P>
// bool is_zero(const MultivariatePolynomial<C,O,P>& p) {
// 	return p.nr_terms() == 0;
// }

/**
 * Check if the polynomial is linear.
 */
template<typename Coeff, typename Ordering, typename Policies>
bool is_constant(const MultivariatePolynomial<Coeff,Ordering,Policies>& p) {
	return (p.nr_terms() == 0) || ((p.nr_terms() == 1) && is_constant(p.lterm()));
}

/**
 * Check if the polynomial is linear.
 */
template<typename Coeff, typename Ordering, typename Policies>
bool is_linear(const MultivariatePolynomial<Coeff,Ordering,Policies>& p) {
	if (carl::is_zero(p)) return true;
	if (Ordering::degreeOrder) {
		return is_linear(p.lterm());
	} else {
		return std::all_of(p.begin(), p.end(),
			[](const auto& t){ return is_linear(t); }
		);
	}
}

/**
 * Returns the total degree of the polynomial, that is the maximum degree of any monomial.
 * As the degree of the zero polynomial is \f$-\infty\f$, we assert that this polynomial is not zero. This must be checked by the caller before calling this method.
 * @see @cite GCL92, page 38
 * @return Total degree.
 */
template<typename Coeff>
std::size_t total_degree(const UnivariatePolynomial<Coeff>& p) {
	if constexpr (carl::is_number_type<Coeff>::value) {
		return p.degree();
	} else {
		if (carl::is_zero(p)) return 0;
		uint max = 0;
		for (std::size_t deg = 0; deg < p.coefficients().size(); deg++) {
			if (!carl::is_zero(p.coefficients()[deg])) {
				uint tdeg = deg + total_degree(p.coefficients()[deg]);
				if (tdeg > max) max = tdeg;
			}
		}
		return max;
	}
}

// /**
//  * Checks if the polynomial is equal to zero.
//  * @return If polynomial is zero.
//  */
// template<typename Coeff>
// bool is_zero(const UnivariatePolynomial<Coeff>& p) {
// 	return p.coefficients().size() == 0;
// }
// 
// /**
//  * Checks if the polynomial is equal to one.
//  * @return If polynomial is one.
//  */
// template<typename Coeff>
// bool is_one(const UnivariatePolynomial<Coeff>& p) {
// 	return p.coefficients().size() == 1 && carl::is_one(p.coefficients().front());
// }

/**
 * Checks whether the polynomial is constant with respect to the main variable.
 * @return If polynomial is constant.
 */
template<typename Coeff>
bool is_constant(const UnivariatePolynomial<Coeff>& p) {
	return p.coefficients().size() <= 1;
}

template<typename Coeff>
bool is_linear(const UnivariatePolynomial<Coeff>& p) {
	if (!p.is_linear_in_main_var()) return false;
	if (!is_linear(p.coefficients().front())) return false;
	bool front = true;
	for (const auto& c : p.coefficients()) {
		if (front) {
			front = false;
			continue;
		}
		if (!is_constant(c)) return false;
	}
	return true;
}

}