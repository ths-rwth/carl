#pragma once

#include "MultivariatePolynomial.h"

namespace carl {

/// @name Equality comparison operators
/// @{
/**
 * Checks if the two arguments are equal.
 * @param lhs First argument.
 * @param rhs Second argument.
 * @return `lhs == rhs`
 */
template<typename C, typename O, typename P>
inline bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	// Try to avoid sorting
	if (&lhs == &rhs) return true;
	if (lhs.nr_terms() != rhs.nr_terms()) return false;
	if (lhs.nr_terms() == 0) return true;
#ifdef THREAD_SAFE
	static std::mutex mutex;
	std::lock_guard<std::mutex> lock(mutex);
#endif
	static std::vector<const C*> coeffs;
	coeffs.resize(MonomialPool::getInstance().largestID() + 1);
	memset(&coeffs[0], 0, sizeof(typename std::vector<const C*>::value_type)*coeffs.size());
	for (const auto& t: lhs) {
		std::size_t id = 0;
		if (t.monomial()) id = t.monomial()->id();
		coeffs[id] = &t.coeff();
	}
	for (const auto& t: rhs) {
		std::size_t id = 0;
		if (t.monomial()) id = t.monomial()->id();
		if ((coeffs[id] == nullptr) || *coeffs[id] != t.coeff()) return false;
	}
	return true;
}
template<typename C, typename O, typename P>
inline bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	if (is_zero(lhs) && is_zero(rhs)) return true;
	if (lhs.nr_terms() > 1) return false;
	return lhs.lterm() == rhs;
}
template<typename C, typename O, typename P>
inline bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
	if (lhs.nr_terms() != 1) return false;
	if (lhs.lmon() != rhs) return false;
	return carl::is_one(lhs.lcoeff());
}
template<typename C, typename O, typename P>
inline bool operator==(const MultivariatePolynomial<C,O,P>& lhs, Variable rhs) {
	if (lhs.nr_terms() != 1) return false;
	if (lhs.lmon() != rhs) return false;
	return carl::is_one(lhs.lcoeff());
}
template<typename C, typename O, typename P>
inline bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	return lhs.is_constant() && lhs.constant_part() == rhs;
}
template<typename C, typename O, typename P, DisableIf<std::is_integral<C>> = dummy>
inline bool operator==(const MultivariatePolynomial<C,O,P>& lhs, int rhs) {
	return lhs.is_constant() && lhs.constant_part() == rhs;
}
template<typename C, typename O, typename P>
inline bool operator==(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs == lhs;
}
template<typename C, typename O, typename P>
inline bool operator==(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs == lhs;
}
template<typename C, typename O, typename P>
inline bool operator==(Variable lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs == lhs;
}
template<typename C, typename O, typename P>
inline bool operator==(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs == lhs;
}

template<typename C, typename O, typename P>
inline bool operator==(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	CARL_LOG_INEFFICIENT();
	return MultivariatePolynomial<C,O,P>(lhs) == rhs;
}
template<typename C, typename O, typename P>
inline bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs) {
	return rhs == lhs;
}
template<typename C, typename O, typename P>
inline bool operator==(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	CARL_LOG_INEFFICIENT();
	return MultivariatePolynomial<C>(lhs) == rhs;
}
template<typename C, typename O, typename P>
inline bool operator==(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs) {
	return rhs == lhs;
}
/// @}
	
/// @name Inequality comparison operators
/// @{
/**
 * Checks if the two arguments are not equal.
 * @param lhs First argument.
 * @param rhs Second argument.
 * @return `lhs != rhs`
 */
template<typename C, typename O, typename P>
inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(lhs == rhs);
}
template<typename C, typename O, typename P>
inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	return !(lhs == rhs);
}
template<typename C, typename O, typename P>
inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
	return !(lhs == rhs);
}
template<typename C, typename O, typename P>
inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, Variable rhs) {
	return !(lhs == rhs);
}
template<typename C, typename O, typename P>
inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	return !(lhs == rhs);
}
template<typename C, typename O, typename P>
inline bool operator!=(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(lhs == rhs);
}
template<typename C, typename O, typename P>
inline bool operator!=(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(lhs == rhs);
}
template<typename C, typename O, typename P>
inline bool operator!=(Variable lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(lhs == rhs);
}
template<typename C, typename O, typename P>
inline bool operator!=(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(lhs == rhs);
}

template<typename C, typename O, typename P>
inline bool operator!=(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(lhs == rhs);
}
template<typename C, typename O, typename P>
inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs) {
	return !(lhs == rhs);
}
template<typename C, typename O, typename P>
inline bool operator!=(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(lhs == rhs);
}
template<typename C, typename O, typename P>
inline bool operator!=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs) {
	return !(lhs == rhs);
}
/// @}

/// @name Less than comparison operators
/// @{
/**
 * Checks if the first arguments is less than the second.
 * @param lhs First argument.
 * @param rhs Second argument.
 * @return `lhs < rhs`
 */
template<typename C, typename O, typename P>
inline bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (is_zero(lhs) && is_zero(rhs)) return false;
	if (is_zero(lhs)) return true;
	if (is_zero(rhs)) return false;
	if (lhs.total_degree() != rhs.total_degree()) return lhs.total_degree() < rhs.total_degree();
	if (lhs.total_degree() == 0) return lhs.constant_part() < rhs.constant_part();
	if (lhs.lterm() < rhs.lterm()) return true;
	if (lhs.lterm() > rhs.lterm()) return false;
	// We have to sort now.
	lhs.makeOrdered();
	rhs.makeOrdered();
	auto lit = lhs.rbegin();
	auto rit = rhs.rbegin();
	for (lit++, rit++; lit != lhs.rend(); lit++, rit++) {
		if (rit == rhs.rend()) return false;
		if (*lit < *rit) return true;
		if (*lit > *rit) return false;
	}
	return rit != rhs.rend();
}
template<typename C, typename O, typename P>
inline bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	if (is_zero(lhs)) return carl::constant_zero<C>().get() < rhs;
	return lhs.lterm() < rhs;
}
template<typename C, typename O, typename P>
inline bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
	if (is_zero(lhs)) return true;
	return lhs.lterm() < rhs;
}
template<typename C, typename O, typename P>
inline bool operator<(const MultivariatePolynomial<C,O,P>& lhs, Variable rhs) {
	if (is_zero(lhs)) return true;
	return lhs.lterm() < rhs;
}
template<typename C, typename O, typename P>
inline bool operator<(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	if (is_zero(lhs)) return true;
	return lhs.lterm() < rhs;
}
template<typename C, typename O, typename P>
inline bool operator<(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (is_zero(rhs)) return lhs < carl::constant_zero<C>().get();
	if (lhs < rhs.lterm()) return true;
	if (lhs == rhs.lterm()) return rhs.nr_terms() > 1;
	return false;
}
template<typename C, typename O, typename P>
inline bool operator<(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (is_zero(rhs)) return false;
	if (lhs < rhs.lterm()) return true;
	if (lhs == rhs.lterm()) return rhs.nr_terms() > 1;
	return false;
}
template<typename C, typename O, typename P>
inline bool operator<(Variable lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (is_zero(rhs)) return false;
	if (lhs < rhs.lterm()) return true;
	if (lhs == rhs.lterm()) return rhs.nr_terms() > 1;
	return false;
}
template<typename C, typename O, typename P>
inline bool operator<(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	if (is_zero(rhs)) return lhs < carl::constant_zero<C>().get();
	return lhs < rhs.lterm();
}
/// @}

/// @name Greater than comparison operators
/// @{
/**
 * Checks if the first argument is greater than the second.
 * @param lhs First argument.
 * @param rhs Second argument.
 * @return `lhs > rhs`
 */
template<typename C, typename O, typename P>
inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs < lhs;
}
template<typename C, typename O, typename P>
inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	return rhs < lhs;
}
template<typename C, typename O, typename P>
inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
	return rhs < lhs;
}
template<typename C, typename O, typename P>
inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, Variable rhs) {
	return rhs < lhs;
}
template<typename C, typename O, typename P>
inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	return rhs < lhs;
}
template<typename C, typename O, typename P>
inline bool operator>(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs < lhs;
}
template<typename C, typename O, typename P>
inline bool operator>(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs < lhs;
}
template<typename C, typename O, typename P>
inline bool operator>(Variable lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs < lhs;
}
template<typename C, typename O, typename P>
inline bool operator>(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs < lhs;
}

template<typename C, typename O, typename P>
inline bool operator>(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs < lhs;
}
template<typename C, typename O, typename P>
inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs) {
	return rhs < lhs;
}
template<typename C, typename O, typename P>
inline bool operator>(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs < lhs;
}
template<typename C, typename O, typename P>
inline bool operator>(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs) {
	return rhs < lhs;
}
/// @}

/// @name Less or equal comparison operators
/// @{
/**
 * Checks if the first argument is less or equal than the second.
 * @param lhs First argument.
 * @param rhs Second argument.
 * @return `lhs <= rhs`
 */
template<typename C, typename O, typename P>
inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(rhs < lhs);
}
template<typename C, typename O, typename P>
inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	return !(rhs < lhs);
}
template<typename C, typename O, typename P>
inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
	return !(rhs < lhs);
}
template<typename C, typename O, typename P>
inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, Variable rhs) {
	return !(rhs < lhs);
}
template<typename C, typename O, typename P>
inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	return !(rhs < lhs);
}
template<typename C, typename O, typename P>
inline bool operator<=(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(rhs < lhs);
}
template<typename C, typename O, typename P>
inline bool operator<=(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(rhs < lhs);
}
template<typename C, typename O, typename P>
inline bool operator<=(Variable lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(rhs < lhs);
}
template<typename C, typename O, typename P>
inline bool operator<=(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(rhs < lhs);
}

template<typename C, typename O, typename P>
inline bool operator<=(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(rhs < lhs);
}
template<typename C, typename O, typename P>
inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs) {
	return !(rhs < lhs);
}
template<typename C, typename O, typename P>
inline bool operator<=(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return !(rhs < lhs);
}
template<typename C, typename O, typename P>
inline bool operator<=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs) {
	return !(rhs < lhs);
}
/// @}

/// @name Greater or equal comparison operators
/// @{
/**
 * Checks if the first argument is greater or equal than the second.
 * @param lhs First argument.
 * @param rhs Second argument.
 * @return `lhs >= rhs`
 */
template<typename C, typename O, typename P>
inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs <= lhs;
}
template<typename C, typename O, typename P>
inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	return rhs <= lhs;
}
template<typename C, typename O, typename P>
inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
	return rhs <= lhs;
}
template<typename C, typename O, typename P>
inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, Variable rhs) {
	return rhs <= lhs;
}
template<typename C, typename O, typename P>
inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	return rhs <= lhs;
}
template<typename C, typename O, typename P>
inline bool operator>=(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs <= lhs;
}
template<typename C, typename O, typename P>
inline bool operator>=(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs <= lhs;
}
template<typename C, typename O, typename P>
inline bool operator>=(Variable lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs <= lhs;
}
template<typename C, typename O, typename P>
inline bool operator>=(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs <= lhs;
}

template<typename C, typename O, typename P>
inline bool operator>=(const UnivariatePolynomial<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs <= lhs;
}
template<typename C, typename O, typename P>
inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<C>& rhs) {
	return rhs <= lhs;
}
template<typename C, typename O, typename P>
inline bool operator>=(const UnivariatePolynomial<MultivariatePolynomial<C>>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return rhs <= lhs;
}
template<typename C, typename O, typename P>
inline bool operator>=(const MultivariatePolynomial<C,O,P>& lhs, const UnivariatePolynomial<MultivariatePolynomial<C>>& rhs) {
	return rhs <= lhs;
}
/// @}

/// @name Addition operators
/// @{

/**
 * Performs an addition involving a polynomial using `operator+=()`.
 * @param lhs First argument.
 * @param rhs Second argument.
 * @return `lhs + rhs`
 */
template<typename C, typename O, typename P>
inline auto operator+(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) += rhs;
}
template<typename C, typename O, typename P>
inline auto operator+(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) += rhs;
}
template<typename C, typename O, typename P>
inline auto operator+(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) += rhs;
}
template<typename C, typename O, typename P>
inline auto operator+(const MultivariatePolynomial<C,O,P>& lhs, Variable rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) += rhs;
}
template<typename C, typename O, typename P>
inline auto operator+(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) += rhs;
}
template<typename C, typename O, typename P>
inline auto operator+(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return MultivariatePolynomial<C>(rhs) += lhs;
}
template<typename C>
inline auto operator+(const Term<C>& lhs, const Term<C>& rhs) {
	return MultivariatePolynomial<C>(lhs) += rhs;
}
template<typename C>
inline auto operator+(const Term<C>& lhs, const Monomial::Arg& rhs) {
	return MultivariatePolynomial<C>(lhs) += rhs;
}
template<typename C>
inline auto operator+(const Term<C>& lhs, Variable rhs) {
	return MultivariatePolynomial<C>(lhs) += rhs;
}
template<typename C>
inline auto operator+(const Term<C>& lhs, const C& rhs) {
	return MultivariatePolynomial<C>(lhs) += rhs;
}
template<typename C, typename O, typename P>
inline auto operator+(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return MultivariatePolynomial<C>(rhs) += lhs;
}
template<typename C>
inline auto operator+(const Monomial::Arg& lhs, const Term<C>& rhs) {
	return MultivariatePolynomial<C>(rhs) += lhs;
}
template<typename C, EnableIf<carl::is_number_type<C>> = dummy>
inline auto operator+(const Monomial::Arg& lhs, const C& rhs) {
	return MultivariatePolynomial<C>(lhs) += rhs;
}
template<typename C, typename O, typename P>
inline auto operator+(Variable lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return MultivariatePolynomial<C>(rhs) += lhs;
}
template<typename C>
inline auto operator+(Variable lhs, const Term<C>& rhs) {
	return MultivariatePolynomial<C>(rhs) += lhs;
}
template<typename C, EnableIf<carl::is_number_type<C>> = dummy>
inline auto operator+(Variable lhs, const C& rhs) {
	return MultivariatePolynomial<C>(lhs) += rhs;
}
template<typename C, typename O, typename P>
inline auto operator+(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return MultivariatePolynomial<C>(rhs) += lhs;
}
template<typename C>
inline auto operator+(const C& lhs, const Term<C>& rhs) {
	return MultivariatePolynomial<C>(rhs) += lhs;
}
template<typename C, EnableIf<carl::is_number_type<C>> = dummy>
inline auto operator+(const C& lhs, const Monomial::Arg& rhs) {
	return MultivariatePolynomial<C>(rhs) += lhs;
}
template<typename C, EnableIf<carl::is_number_type<C>> = dummy>
inline auto operator+(const C& lhs, Variable rhs) {
	return MultivariatePolynomial<C>(rhs) += lhs;
}
/// @}

/// @name Subtraction operators
/// @{
/**
 * Performs a subtraction involving a polynomial using `operator-=()`.
 * @param lhs First argument.
 * @param rhs Second argument.
 * @return `lhs - rhs`
 */
template<typename C, typename O, typename P>
inline auto operator-(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) -= rhs;
}
template<typename C, typename O, typename P>
inline auto operator-(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) -= rhs;
}
template<typename C, typename O, typename P>
inline auto operator-(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) -= rhs;
}
template<typename C, typename O, typename P>
inline auto operator-(const MultivariatePolynomial<C,O,P>& lhs, Variable rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) -= rhs;
}
template<typename C, typename O, typename P>
inline auto operator-(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) -= rhs;
}
template<typename C, typename O, typename P>
inline auto operator-(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return -rhs + lhs;
}
template<typename C>
inline auto operator-(const Term<C>& lhs, const Term<C>& rhs) {
	return MultivariatePolynomial<C>(lhs) -= rhs;
}
template<typename C>
inline auto operator-(const Term<C>& lhs, const Monomial::Arg& rhs) {
	return MultivariatePolynomial<C>(lhs) -= rhs;
}
template<typename C>
inline auto operator-(const Term<C>& lhs, Variable rhs) {
	return MultivariatePolynomial<C>(lhs) -= rhs;
}
template<typename C>
inline auto operator-(const Term<C>& lhs, const C& rhs) {
	return MultivariatePolynomial<C>(lhs) -= rhs;
}
template<typename C, typename O, typename P>
inline auto operator-(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return -rhs + lhs;
}
template<typename C>
inline auto operator-(const Monomial::Arg& lhs, const Term<C>& rhs) {
	return -rhs + lhs;
}
template<typename C, EnableIf<carl::is_number_type<C>> = dummy>
inline auto operator-(const Monomial::Arg& lhs, const C& rhs) {
	return MultivariatePolynomial<C>(lhs) -= rhs;
}
template<typename C, typename O, typename P>
inline auto operator-(Variable lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return -rhs + lhs;
}
template<typename C>
inline auto operator-(Variable lhs, const Term<C>& rhs) {
	return -rhs + lhs;
}
template<typename C, EnableIf<carl::is_number_type<C>> = dummy>
inline auto operator-(Variable lhs, const C& rhs) {
	return MultivariatePolynomial<C>(lhs) -= rhs;
}
template<typename C, typename O, typename P>
inline auto operator-(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return -rhs + lhs;
}
template<typename C>
inline auto operator-(const C& lhs, const Term<C>& rhs) {
	return -rhs + lhs;
}
template<typename C, EnableIf<carl::is_number_type<C>> = dummy>
inline auto operator-(const C& lhs, const Monomial::Arg& rhs) {
	return -MultivariatePolynomial<C>(rhs) + lhs;
}
template<typename C, EnableIf<carl::is_number_type<C>> = dummy>
inline auto operator-(const C& lhs, Variable rhs) {
	return -MultivariatePolynomial<C>(rhs) + lhs;
}
/// @}
	
/// @name Multiplication operators
/// @{
/**
 * Perform a multiplication involving a polynomial using `operator*=()`.
 * @param lhs Left hand side.
 * @param rhs Right hand side.
 * @return `lhs * rhs`
 */
template<typename C, typename O, typename P>
inline auto operator*(const MultivariatePolynomial<C,O,P>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) *= rhs;
}
template<typename C, typename O, typename P>
inline auto operator*(const MultivariatePolynomial<C,O,P>& lhs, const Term<C>& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) *= rhs;
}
template<typename C, typename O, typename P>
inline auto operator*(const MultivariatePolynomial<C,O,P>& lhs, const Monomial::Arg& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) *= rhs;
}
template<typename C, typename O, typename P>
inline auto operator*(const MultivariatePolynomial<C,O,P>& lhs, Variable rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) *= rhs;
}
template<typename C, typename O, typename P>
inline auto operator*(const MultivariatePolynomial<C,O,P>& lhs, const C& rhs) {
	return MultivariatePolynomial<C,O,P>(lhs) *= rhs;
}
template<typename C, typename O, typename P>
inline auto operator*(const Term<C>& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return MultivariatePolynomial<C,O,P>(rhs) *= lhs;
}
template<typename C, typename O, typename P>
inline auto operator*(const Monomial::Arg& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return MultivariatePolynomial<C,O,P>(rhs) *= lhs;
}
template<typename C, typename O, typename P>
inline auto operator*(Variable lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return MultivariatePolynomial<C,O,P>(rhs) *= lhs;
}
template<typename C, typename O, typename P>
inline auto operator*(const C& lhs, const MultivariatePolynomial<C,O,P>& rhs) {
	return MultivariatePolynomial<C,O,P>(rhs) *= lhs;
}
/// @}
	

}