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
#include <limits.h>
#include <sstream>
#include <vector>
#include "constants.h"

#include "z3_include.h"

namespace carl {

inline mpz_manager<true>& mpzmanager() {
	static mpz_manager<true> m;
	return m;
}
inline mpq_manager<true>& mpqmanager() {
	static mpq_manager<true> m;
	return m;
}

/**
 * Informational functions
 * 
 * The following functions return informations about the given numbers.
 */
inline bool isZero(const rational& n) {
	return n.is_zero();
}
inline bool isOne(const rational& n) {
	return n.is_one();
}

	
inline mpz getNum(const mpq& n) {
	return n.numerator();
}

inline mpz getNum(const mpz& n) {
	return n;
}

inline mpz getDenom(const mpq& n) {
	return n.denominator();
}

inline mpz getDenom(const mpz& n) {
	return n;
}

inline bool isInteger(const mpq& n) {
	return mpqmanager().is_one(n.denominator());
}

inline bool isInteger(const mpz&) {
	return true;
}

/**
 * Conversion functions
 * 
 * The following function convert types to other types.
 */

inline double toDouble(const mpq& n) {
	return mpqmanager().get_double(n);
}
inline double toDouble(const mpz& n) {
	return mpzmanager().get_double(n);
}

template<typename Integer>
inline Integer toInt(const mpz& n);
template<>
inline signed long int toInt<signed long int>(const mpz& n) {
    assert(n <= INT_MAX);
    return mpzmanager().get_int64(n);
}
template<>
inline unsigned long int toInt<unsigned long int>(const mpz& n) {
    assert(n <= UINT_MAX);
    return mpzmanager().get_uint64(n);
}

template<typename T>
inline T rationalize(double n);

template<>
inline mpq rationalize<mpq>(double d) {
	return mpq(d);
}

/**
 * Basic Operators
 * 
 * The following functions implement simple operations on the given numbers.
 */

inline mpz abs(const mpz& n) {
	mpz res = n;
	mpzmanager().abs(res);
	return res;
}

inline mpq abs(const mpq& n) {
	mpq res = n;
	mpqmanager().abs(res);
	return res;
}

inline mpz floor(const mpq& n) {
	mpz res;
	mpqmanager().floor(n, res);
	return res;
}
inline mpz floor(const mpz& n) {
	return n;
}

inline mpz ceil(const mpq& n) {
	mpz res;
	mpqmanager().ceil(n, res);
	return res;
}
inline mpz ceil(const mpz& n) {
	return n;
}

inline mpz gcd(const mpz& a, const mpz& b) {
	mpz res;
	mpzmanager().gcd(a, b, res);
	return res;
}

inline mpz lcm(const mpz& a, const mpz& b) {
	mpz res;
	mpzmanager().lcm(a, b, res);
	return res;
}

inline mpz gcd(const mpq& a, const mpq& b) {
    assert( isInteger(a) );
    assert( isInteger(b) );
	mpz res;
	mpzmanager().gcd(a.numerator(), b.numerator(), res);
	return res;
}

inline mpz lcm(const mpq& a, const mpq& b) {
    assert( isInteger(a) );
    assert( isInteger(b) );
	mpz res;
	mpzmanager().lcm(a.numerator(), b.numerator(), res);
	return res;
}

inline mpz pow(const mpz& b, unsigned e) {
	mpz res;
	mpzmanager().power(b, e, res);
	return res;
}

mpq pow(const mpq& b, unsigned e);

std::pair<mpq,mpq> sqrt(const mpq& a);

inline mpz mod(const mpz& n, const mpz& m) {
	mpz res;
	mpzmanager().mod(n, m, res);
	return res;
}

inline mpz quotient(const mpz& n, const mpz& d)
{
	mpz res;
	mpzmanager().div(n, d, res);
	return res;
}

inline mpz operator/(const mpz& n, const mpz& d)
{
	return quotient(n,d);
}

inline void divide(const mpz& dividend, const mpz& divisor, mpz& quotient, mpz& remainder) {
	mpzmanager().div(dividend, divisor, quotient);
	mpzmanager().rem(dividend, divisor, remainder);
}

inline mpq operator *(const mpq& lhs, const mpq& rhs)
{
	mpq res;
	mpqmanager().mul(lhs, rhs, res);
	return res;
}

inline mpq operator /(const mpq& lhs, const mpq& rhs)
{
	mpq res;
	mpqmanager().div(lhs, rhs, res);
	return res;
}

template<typename T>
inline T rationalize(const std::string& n);

template<>
mpq rationalize<mpq>(const std::string& inputstring);

inline std::string toString(const mpq& _number, bool _infix) {
	return mpqmanager().to_string(_number);
}

inline std::string toString(const mpz& _number, bool _infix) {
	return mpzmanager().to_string(_number);
}
inline std::string toString(const rational& _number, bool _infix) {
	return toString(_number.to_mpq(), _infix);
}

}

