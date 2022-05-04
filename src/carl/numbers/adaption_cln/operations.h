/**
 * @file   adaption_cln/operations.h
 * @ingroup cln
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Sebastian Junges
 *
 * @warning This file should never be included directly but only via operations.h
 *
 */
#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include <carl-common/meta/platform.h>
#include "typetraits.h"
#include <cassert>
#include <limits>

namespace carl {

inline bool isZero(const cln::cl_I& n) {
	return zerop(n);
}

inline bool isZero(const cln::cl_RA& n) {
	return zerop(n);
}

inline bool isOne(const cln::cl_I& n) {
	return n == constant_one<cln::cl_I>::get();
}

inline bool isOne(const cln::cl_RA& n) {
	return n  == constant_one<cln::cl_RA>::get();
}

inline bool isPositive(const cln::cl_I& n) {
	return n > constant_zero<cln::cl_RA>::get();
}

inline bool isPositive(const cln::cl_RA& n) {
	return n > constant_zero<cln::cl_RA>::get();
}

inline bool isNegative(const cln::cl_I& n) {
	return n < constant_zero<cln::cl_RA>::get();
}

inline bool isNegative(const cln::cl_RA& n) {
	return n < constant_zero<cln::cl_RA>::get();
}

/**
 * Extract the numerator from a fraction.
 * @param n Fraction.
 * @return Numerator.
 */
inline cln::cl_I getNum(const cln::cl_RA& n) {
	return cln::numerator(n);
}

/**
 * Extract the denominator from a fraction.
 * @param n Fraction.
 * @return Denominator.
 */
inline cln::cl_I getDenom(const cln::cl_RA& n) {
	return cln::denominator(n);
}

/**
 * Check if a number is integral.
 * As cln::cl_I are always integral, this method returns true.
 * @return true.
 */
inline bool isInteger(const cln::cl_I& /*unused*/) {
	return true;
}

/**
 * Check if a fraction is integral.
 * @param n A fraction.
 * @return true.
 */
inline bool isInteger(const cln::cl_RA& n) {
	return isOne(getDenom(n));
}

/**
 * Get the bit size of the representation of a integer.
 * @param n An integer.
 * @return Bit size of n.
 */
inline std::size_t bitsize(const cln::cl_I& n) {
	return cln::integer_length(n);
}
/**
 * Get the bit size of the representation of a fraction.
 * @param n A fraction.
 * @return Bit size of n.
 */
inline std::size_t bitsize(const cln::cl_RA& n) {
	return cln::integer_length(getNum(n)) + cln::integer_length(getDenom(n));
}

/**
 * Converts the given fraction to a double.
 * @param n A fraction.
 * @return Double.
 */
inline double toDouble(const cln::cl_RA& n) {
	return cln::double_approx(n);
}
/**
 * Converts the given integer to a double.
 * @param n An integer.
 * @return Double.
 */
inline double toDouble(const cln::cl_I& n) {
	return cln::double_approx(n);
}

template<typename Integer>
inline Integer toInt(const cln::cl_I& n);
template<typename Integer>
inline Integer toInt(const cln::cl_RA& n);

template<>
inline sint toInt<sint>(const cln::cl_I& n) {
    assert(n <= std::numeric_limits<sint>::max());
    assert(n >= std::numeric_limits<sint>::min());
    return cln::cl_I_to_long(n);
}
template<>
inline uint toInt<uint>(const cln::cl_I& n) {
    assert(n <= std::numeric_limits<uint>::max());
    assert(n >= std::numeric_limits<uint>::min());
    return uint(cln::cl_I_to_long(n));
}


template<typename To, typename From>
inline To fromInt(const From& n);

template<>
inline cln::cl_I fromInt(const uint& n) {
	return cln::cl_I(n);
}

template<>
inline cln::cl_I fromInt(const sint& n) {
	return cln::cl_I(n);
}

template<>
inline cln::cl_RA fromInt(const uint& n) {
	return cln::cl_RA(n);
}

template<>
inline cln::cl_RA fromInt(const sint& n) {
	return cln::cl_RA(n);
}

/**
 * Convert a fraction to an integer.
 * This method assert, that the given fraction is an integer, i.e. that the denominator is one.
 * @param n A fraction.
 * @return An integer.
 */
template<>
inline cln::cl_I toInt<cln::cl_I>(const cln::cl_RA& n) {
	assert(isInteger(n));
	return getNum(n);
}
template<>
inline sint toInt<sint>(const cln::cl_RA& n) {
	return toInt<sint>(toInt<cln::cl_I>(n));
}
template<>
inline uint toInt<uint>(const cln::cl_RA& n) {
    return toInt<uint>(toInt<cln::cl_I>(n));
}

/**
 * Convert a cln fraction to a cln long float.
 * @param n A fraction.
 * @return n as cln::cl_LF.
 */
inline cln::cl_LF toLF(const cln::cl_RA& n) {
	return cln::cl_R_to_LF(n, std::max(cln::integer_length(cln::numerator(n)), cln::integer_length(cln::denominator(n))));
}

static const cln::cl_RA ONE_DIVIDED_BY_10_TO_THE_POWER_OF_23 = cln::cl_RA(1)/cln::expt(cln::cl_RA(10), 23);
static const cln::cl_RA ONE_DIVIDED_BY_10_TO_THE_POWER_OF_52 = cln::cl_RA(1)/cln::expt(cln::cl_RA(10), 52);

template<>
cln::cl_RA rationalize<cln::cl_RA>(double n);

template<>
cln::cl_RA rationalize<cln::cl_RA>(float n);

template<>
inline cln::cl_RA rationalize<cln::cl_RA>(int n) {
    return cln::cl_RA(n);
}

template<>
inline cln::cl_RA rationalize<cln::cl_RA>(uint n) {
	return cln::cl_RA(n);
}

template<>
inline cln::cl_RA rationalize<cln::cl_RA>(sint n) {
	return cln::cl_RA(n);
}

template<>
cln::cl_I parse<cln::cl_I>(const std::string& n);

template<>
bool try_parse<cln::cl_I>(const std::string& n, cln::cl_I& res);

template<>
cln::cl_RA parse<cln::cl_RA>(const std::string& n);

template<>
bool try_parse<cln::cl_RA>(const std::string& n, cln::cl_RA& res);

/**
 * Get absolute value of an integer.
 * @param n An integer.
 * @return \f$|n|\f$.
 */
inline cln::cl_I abs(const cln::cl_I& n) {
	return cln::abs(n);
}

/**
 * Get absolute value of a fraction.
 * @param n A fraction.
 * @return \f$|n|\f$.
 */
inline cln::cl_RA abs(const cln::cl_RA& n) {
	return cln::abs(n);
}

/**
 * Round a fraction to next integer.
 * @param n A fraction.
 * @return The next integer.
 */
inline cln::cl_I round(const cln::cl_RA& n) {
	return cln::round1(n);
}

/**
 * Round an integer to next integer, that is do nothing.
 * @param n An integer.
 * @return The next integer.
 */
inline cln::cl_I round(const cln::cl_I& n) {
	return n;
}

/**
 * Round down a fraction.
 * @param n A fraction.
 * @return \f$\lfloor n \rfloor\f$.
 */
inline cln::cl_I floor(const cln::cl_RA& n) {
	return cln::floor1(n);
}

/**
 * Round down an integer.
 * @param n An integer.
 * @return \f$\lfloor n \rfloor\f$.
 */
inline cln::cl_I floor(const cln::cl_I& n) {
	return n;
}

/**
 * Round up a fraction.
 * @param n A fraction.
 * @return \f$\lceil n \rceil\f$.
 */
inline cln::cl_I ceil(const cln::cl_RA& n) {
	return cln::ceiling1(n);
}

/**
 * Round up an integer.
 * @param n An integer.
 * @return \f$\lceil n \rceil\f$.
 */
inline cln::cl_I ceil(const cln::cl_I& n) {
	return n;
}

/**
 * Calculate the greatest common divisor of two integers.
 * @param a First argument.
 * @param b Second argument.
 * @return Gcd of a and b.
 */
inline cln::cl_I gcd(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::gcd(a,b);
}

/**
 * Calculate the greatest common divisor of two integers.
 * Stores the result in the first argument.
 * @param a First argument.
 * @param b Second argument.
 * @return Updated a.
 */
inline cln::cl_I& gcd_assign(cln::cl_I& a, const cln::cl_I& b) {
    a = cln::gcd(a,b);
	return a;
}

inline void divide(const cln::cl_I& dividend, const cln::cl_I& divisor, cln::cl_I& quotient, cln::cl_I& remainder) {
	cln::cl_I_div_t res = cln::floor2(dividend, divisor);
    quotient = res.quotient;
    remainder = res.remainder;
}

/**
 * Calculate the greatest common divisor of two fractions.
 * Stores the result in the first argument.
 * Asserts that the arguments are integral.
 * @param a First argument.
 * @param b Second argument.
 * @return Updated a.
 */
inline cln::cl_RA& gcd_assign(cln::cl_RA& a, const cln::cl_RA& b) {
	a = cln::gcd(carl::getNum(a),carl::getNum(b)) / cln::lcm(carl::getDenom(a),carl::getDenom(b));
	return a;
}

/**
 * Calculate the greatest common divisor of two fractions.
 * Asserts that the arguments are integral.
 * @param a First argument.
 * @param b Second argument.
 * @return Gcd of a and b.
 */
inline cln::cl_RA gcd(const cln::cl_RA& a, const cln::cl_RA& b) {
	return cln::gcd(carl::getNum(a),carl::getNum(b)) / cln::lcm(carl::getDenom(a),carl::getDenom(b));
}

/**
 * Calculate the least common multiple of two integers.
 * @param a First argument.
 * @param b Second argument.
 * @return Lcm of a and b.
 */
inline cln::cl_I lcm(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::lcm(a,b);
}

/**
 * Calculate the least common multiple of two fractions.
 * Asserts that the arguments are integral.
 * @param a First argument.
 * @param b Second argument.
 * @return Lcm of a and b.
 */
inline cln::cl_RA lcm(const cln::cl_RA& a, const cln::cl_RA& b) {
    assert( carl::isInteger( a ) );
    assert( carl::isInteger( b ) );
	return cln::lcm(carl::getNum(a),carl::getNum(b));
}

/**
 * Calculate the power of some fraction to some positive integer.
 * @param basis Basis.
 * @param exp Exponent.
 * @return \f$n^e\f$
 */
template<>
inline cln::cl_RA pow(const cln::cl_RA& basis, std::size_t exp) {
	return cln::expt(basis, int(exp));
}

inline cln::cl_RA log(const cln::cl_RA& n) {
	return cln::rationalize(cln::realpart(cln::log(n)));
}
inline cln::cl_RA log10(const cln::cl_RA& n) {
	return cln::rationalize(cln::realpart(cln::log(n, 10)));
}

inline cln::cl_RA sin(const cln::cl_RA& n) {
	return cln::rationalize(cln::sin(n));
}

inline cln::cl_RA cos(const cln::cl_RA& n) {
	return cln::rationalize(cln::cos(n));
}

/**
 * Calculate the square root of a fraction if possible.
 *
 * @param a The fraction to calculate the square root for.
 * @param b A reference to the rational, in which the result is stored.
 * @return true, if the number to calculate the square root for is a square;
 *         false, otherwise.
 */
bool sqrt_exact(const cln::cl_RA& a, cln::cl_RA& b);

cln::cl_RA sqrt(const cln::cl_RA& a);

/**
 * Calculate the square root of a fraction.
 *
 * If we are able to find a an \f$x\f$ such that \f$x\f$ is the exact root of \f$a\f$, \f$(x,x)\f$ is returned.
 * If we can not find such a number (note that such a number might not even exist), \f$(x,y)\f$ is returned with \f$ x < \sqrt{a} < y \f$.
 * Note that we try to find bounds that are very close to the actual square root. If a small representation is more important than a small interval, sqrt_fast should be used.
 * @param a A fraction.
 * @return Interval containing the square root of a.
 */
std::pair<cln::cl_RA, cln::cl_RA> sqrt_safe(const cln::cl_RA& a);

/**
 * Compute square root in a fast but less precise way.
 * Use cln::sqrt() to obtain an approximation. If the result is rational, i.e. the result is exact, use this result.
 * Otherwise use the nearest integers as bounds on the square root.
 * @param a Some number.
 * @return [x,x] if sqrt(a) = x is rational, otherwise [y,z] for y,z integer and y < sqrt(a) < z.
 */
std::pair<cln::cl_RA, cln::cl_RA> sqrt_fast(const cln::cl_RA& a);

std::pair<cln::cl_RA, cln::cl_RA> root_safe(const cln::cl_RA& a, uint n);

/**
 * Calculate the remainder of the integer division.
 * @param a First argument.
 * @param b Second argument.
 * @return \f$a \% b\f$.
 */
inline cln::cl_I mod(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::rem(a, b);
}

/**
 * Divide two fractions.
 * @param a First argument.
 * @param b Second argument.
 * @return \f$ a / b \f$.
 */
inline cln::cl_RA div(const cln::cl_RA& a, const cln::cl_RA& b) {
	return (a / b);
}

/**
 * Divide two integers.
 * Asserts that the remainder is zero.
 * @param a First argument.
 * @param b Second argument.
 * @return \f$ a / b \f$.
 */
inline cln::cl_I div(const cln::cl_I& a, const cln::cl_I& b) {
	assert(cln::mod(a, b) == 0);
	return cln::exquo(a, b);
}

/**
 * Divide two fractions.
 * Stores the result in the first argument.
 * @param a First argument.
 * @param b Second argument.
 * @return \f$ a / b \f$.
 */
inline cln::cl_RA& div_assign(cln::cl_RA& a, const cln::cl_RA& b) {
    a /= b;
	return a;
}

/**
 * Divide two integers.
 * Asserts that the remainder is zero.
 * Stores the result in the first argument.
 * @param a First argument.
 * @param b Second argument.
 * @return \f$ a / b \f$.
 */
inline cln::cl_I& div_assign(cln::cl_I& a, const cln::cl_I& b) {
	assert(cln::mod(a,b) == 0);
	a = cln::exquo(a, b);
    return a;
}

/**
 * Divide two fractions.
 * @param a First argument.
 * @param b Second argument.
 * @return \f$ a / b \f$.
 */
inline cln::cl_RA quotient(const cln::cl_RA& a, const cln::cl_RA& b)
{
	return a / b;
}
/**
 * Divide two integers.
 * Discards the remainder of the division.
 * @param a First argument.
 * @param b Second argument.
 * @return \f$ a / b \f$.
 */
inline cln::cl_I quotient(const cln::cl_I& a, const cln::cl_I& b)
{
	return cln::exquo(a - cln::rem(a, b), b);
}


/**
 * Calculate the remainder of the integer division.
 * @param a First argument.
 * @param b Second argument.
 * @return \f$a \% b\f$.
 */
inline cln::cl_I remainder(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::rem(a, b);
}


/**
 * Divide two integers.
 * Discards the remainder of the division.
 * @param a First argument.
 * @param b Second argument.
 * @return \f$ a / b \f$.
 */
inline cln::cl_I operator/(const cln::cl_I& a, const cln::cl_I& b)
{
	return quotient(a,b);
}
inline cln::cl_I operator/(const cln::cl_I& lhs, const int& rhs) {
	return lhs / cln::cl_I(rhs);
}

inline cln::cl_RA reciprocal(const cln::cl_RA& a) {
	return cln::recip(a);
}

std::string toString(const cln::cl_RA& _number, bool _infix=true);

std::string toString(const cln::cl_I& _number, bool _infix=true);

}
