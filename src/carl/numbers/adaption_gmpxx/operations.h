/** 
 * @file   adaption_gmpxx/operations.h
 * @ingroup gmpxx
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * 
 * @warning This file should never be included directly but only via operations.h
 */

#pragma once
#include "../../util/platform.h"
#include <cstddef>
CLANG_WARNING_DISABLE("-Wsign-conversion")
#include <gmpxx.h>
CLANG_WARNING_RESET
#include <limits.h>
#include <sstream>
#include <vector>
#include "boost/algorithm/string.hpp"
#include "../constants.h"

namespace carl {


/**
 * Informational functions
 * 
 * The following functions return informations about the given numbers.
 */
inline bool isZero(const mpz_class& n) {
	return constant_zero<mpz_class>::get() == n;
}

inline bool isZero(const mpq_class& n) {
	return constant_zero<mpz_class>::get() == n;
}

inline bool isOne(const mpz_class& n) {
	return constant_one<mpz_class>::get() == n;
}

inline bool isOne(const mpq_class& n) {
	return constant_one<mpz_class>::get() == n;
}

	
inline mpz_class getNum(const mpq_class& n) {
	return n.get_num();
}

inline mpz_class getNum(const mpz_class& n) {
	return n;
}

inline mpz_class getDenom(const mpq_class& n) {
	return n.get_den();
}

inline mpz_class getDenom(const mpz_class& n) {
	return n;
}

inline bool isInteger(const mpq_class& n) {
	 return 0 != mpz_divisible_p(n.get_num_mpz_t(), n.get_den_mpz_t());
}

inline bool isInteger(const mpz_class&) {
	return true;
}

/**
 * Conversion functions
 * 
 * The following function convert types to other types.
 */

inline double toDouble(const mpq_class& n) {
	return n.get_d();
}
inline double toDouble(const mpz_class& n) {
	return n.get_d();
}

template<typename Integer>
inline Integer toInt(const mpz_class& n);
template<>
inline signed long int toInt<signed long int>(const mpz_class& n) {
    assert(n <= INT_MAX);
    return mpz_get_si(n.get_mpz_t());
}
template<>
inline unsigned long int toInt<unsigned long int>(const mpz_class& n) {
    assert(n <= UINT_MAX);
    return mpz_get_ui(n.get_mpz_t());
}

template<typename T>
inline T rationalize(double n);

template<>
inline mpq_class rationalize<mpq_class>(double d) {
	return mpq_class(d);
}

/**
 * Basic Operators
 * 
 * The following functions implement simple operations on the given numbers.
 */

inline mpz_class abs(const mpz_class& n) {
	mpz_class res;
	mpz_abs(res.get_mpz_t(), n.get_mpz_t());
	return res;
}

inline mpq_class abs(const mpq_class& n) {
	mpq_class res;
	mpq_abs(res.get_mpq_t(), n.get_mpq_t());
	return res;
}

inline mpz_class floor(const mpq_class& n) {
	mpz_class res;
	mpz_fdiv_q(res.get_mpz_t(), n.get_den_mpz_t(), n.get_num_mpz_t());
	return res;
}
inline mpz_class floor(const mpz_class& n) {
	return n;
}

inline mpz_class ceil(const mpq_class& n) {
	mpz_class res;
	mpz_cdiv_q(res.get_mpz_t(), n.get_den_mpz_t(), n.get_num_mpz_t());
	return res;
}
inline mpz_class ceil(const mpz_class& n) {
	return n;
}

inline mpz_class gcd(const mpz_class& a, const mpz_class& b) {
	mpz_class res;
	mpz_gcd(res.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
	return res;
}

inline mpz_class lcm(const mpz_class& a, const mpz_class& b) {
	mpz_class res;
	mpz_lcm(res.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t());
	return res;
}

inline mpz_class gcd(const mpq_class& a, const mpq_class& b) {
    assert( isInteger(a) );
    assert( isInteger(b) );
	mpz_class res;
	mpz_gcd(res.get_mpz_t(), getNum(a).get_mpz_t(), getNum(b).get_mpz_t());
	return res;
}

inline mpz_class lcm(const mpq_class& a, const mpq_class& b) {
    assert( isInteger(a) );
    assert( isInteger(b) );
	mpz_class res;
	mpz_lcm(res.get_mpz_t(), getNum(a).get_mpz_t(), getNum(b).get_mpz_t());
	return res;
}

inline mpz_class pow(const mpz_class& b, unsigned e) {
	mpz_class res;
	mpz_pow_ui(res.get_mpz_t(), b.get_mpz_t(), e);
	return res;
}

mpq_class pow(const mpq_class& b, unsigned e);

/**
 * Calculate the square root of a fraction if possible.
 * 
 * @param a The fraction to calculate the square root for.
 * @param b A reference to the rational, in which the result is stored.
 * @return true, if the number to calculate the square root for is a square;
 *         false, otherwise.
 */
bool sqrtp(const mpq_class& a, mpq_class& b);

std::pair<mpq_class,mpq_class> sqrt(const mpq_class& a);

inline mpz_class mod(const mpz_class& n, const mpz_class& m) {
	mpz_class res;
	mpz_mod(res.get_mpz_t(), n.get_mpz_t(), m.get_mpz_t());
	return res;
}

inline mpz_class quotient(const mpz_class& n, const mpz_class& d)
{
	mpz_class res;
	mpz_div(res.get_mpz_t(), n.get_mpz_t(), d.get_mpz_t());
	return res;
}

inline mpz_class operator/(const mpz_class& n, const mpz_class& d)
{
	return quotient(n,d);
}

inline void divide(const mpz_class& dividend, const mpz_class& divisor, mpz_class& quotient, mpz_class& remainder) {
	mpz_divmod(quotient.get_mpz_t(), remainder.get_mpz_t(), dividend.get_mpz_t(), divisor.get_mpz_t());
}

inline mpq_class operator *(const mpq_class& lhs, const mpq_class& rhs)
{
	mpq_t res;
	mpq_init(res);
	mpq_mul(res, lhs.get_mpq_t(), rhs.get_mpq_t());
	return mpq_class(res);
}

inline mpq_class operator /(const mpq_class& lhs, const mpq_class& rhs)
{
	mpq_t res;
	mpq_init(res);
	mpq_div(res, lhs.get_mpq_t(), rhs.get_mpq_t());
	return mpq_class(res);
}

template<typename T>
inline T rationalize(const std::string& n);

template<>
mpq_class rationalize<mpq_class>(const std::string& inputstring);

std::string toString(const mpq_class& _number, bool _infix);

std::string toString(const mpz_class& _number, bool _infix);

}

