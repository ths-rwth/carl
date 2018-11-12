#pragma once

#include "MultivariatePolynomial.h"

namespace carl {

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
template<typename C, EnableIf<carl::is_number<C>> = dummy>
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
template<typename C, EnableIf<carl::is_number<C>> = dummy>
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
template<typename C, EnableIf<carl::is_number<C>> = dummy>
inline auto operator+(const C& lhs, const Monomial::Arg& rhs) {
	return MultivariatePolynomial<C>(rhs) += lhs;
}
template<typename C, EnableIf<carl::is_number<C>> = dummy>
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
template<typename C, EnableIf<carl::is_number<C>> = dummy>
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
template<typename C, EnableIf<carl::is_number<C>> = dummy>
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
template<typename C, EnableIf<carl::is_number<C>> = dummy>
inline auto operator-(const C& lhs, const Monomial::Arg& rhs) {
	return -MultivariatePolynomial<C>(rhs) + lhs;
}
template<typename C, EnableIf<carl::is_number<C>> = dummy>
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