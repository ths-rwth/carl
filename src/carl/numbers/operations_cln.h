/* 
 * File:   operations_cln.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <cassert>
#include <climits>

#include "typetraits.h"

namespace carl {

/**
 * Informational functions
 * 
 * The following functions return informations about the given numbers.
 */

template<>
inline cln::cl_I getNum(const cln::cl_RA& n) {
	return cln::numerator(n);
}

template<>
inline cln::cl_I getDenom(const cln::cl_RA& n) {
	return cln::denominator(n);
}

template<>
inline bool isInteger(const cln::cl_I&) {
	return true;
}
template<>
inline bool isInteger(const cln::cl_RA& n) {
	return getDenom(n) == (cln::cl_I)(1);
}

/**
 * Conversion functions
 * 
 * The following function convert types to other types.
 */
template<>
inline double toDouble(const cln::cl_RA& n) {
	return cln::double_approx(n);
}
template<>
inline double toDouble(const cln::cl_I& n) {
	return cln::double_approx(n);
}

template<>
inline int toInt(const cln::cl_I& n) {
    assert(n <= INT_MAX);
    return cln::cl_I_to_int(n);
}
template<>
inline unsigned toInt(const cln::cl_I& n) {
    assert(n <= UINT_MAX);
    return cln::cl_I_to_uint(n);
}

template<>
inline cln::cl_RA rationalize<cln::cl_RA>(double n) {
	return cln::rationalize(cln::cl_R(n));
}

/**
 * Basic Operators
 * 
 * The following functions implement simple operations on the given numbers.
 */
template<>
inline cln::cl_I floor(const cln::cl_RA& n) {
	return cln::floor1(n);
}

template<>
inline cln::cl_I ceil(const cln::cl_RA& n) {
	return cln::ceiling1(n);
}

template<>
inline cln::cl_I gcd(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::gcd(a,b);
}

template<>
inline cln::cl_I lcm(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::lcm(a,b);
}

template<>
inline cln::cl_RA pow(const cln::cl_RA& n, unsigned e) {
	return cln::expt(n, (int)e);
}

template<>
inline cln::cl_I mod(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::mod(a, b);
}

template<>
inline cln::cl_I div(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::floor1(a / b);
}

}