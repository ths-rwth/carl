/** 
 * @file   adaption_gmpxx/operations.h
 * @ingroup gmpxx
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * 
 * @warning This file should never be included directly but only via operations.h
 */

#pragma once
#include <gmpxx.h>

namespace carl {

/**
 * Informational functions
 * 
 * The following functions return informations about the given numbers.
 */

inline const mpz_class& getNum(const mpq_class& n) {
	return n.get_num();
}
inline const mpz_class& getNum(const mpz_class& n) {
	return n;
}

inline const mpz_class& getDenom(const mpq_class& n) {
	return n.get_den();
}
inline const mpz_class& getDenom(const mpz_class& n) {
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

inline mpz_class pow(const mpz_class& b, unsigned e) {
	mpz_class res;
	mpz_pow_ui(res.get_mpz_t(), b.get_mpz_t(), e);
	return res;
}

inline std::pair<mpq_class,mpq_class> sqrt(const mpq_class& a) {
    assert( mpq_sgn(a.__get_mp()) > 0 );
    mpz_class den = a.get_den();
    mpz_class num = a.get_num();
    mpz_class root_den;
    mpz_class root_den_rem;
    mpz_sqrtrem(root_den.__get_mp(), root_den_rem.__get_mp(), den.__get_mp());
    
    mpz_class root_num;
    mpz_class root_num_rem;
    mpz_sqrtrem(root_num.__get_mp(), root_num_rem.__get_mp(), num.__get_mp());
    
    mpq_class lower;
    mpq_class upper;
    long unsigned numerator, denominator;
    
    numerator = mpz_get_ui(root_num.__get_mp());
    denominator = mpz_get_ui(root_den.__get_mp());
    
    mpq_set_ui(upper.__get_mp(), mpz_sgn(root_den_rem.__get_mp()) != 0 ? denominator+1 : denominator, numerator);
    mpq_set_ui(lower.__get_mp(), denominator, mpz_sgn(root_num_rem.__get_mp()) != 0 ? numerator+1 : numerator );
    return std::make_pair(lower,upper);
}

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


}

