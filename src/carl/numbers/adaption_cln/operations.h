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

#include <cassert>
#include <limits>
#include <cmath>

namespace carl {

/**
 * Informational functions
 * 
 * The following functions return informations about the given numbers.
 */

inline const cln::cl_I getNum(const cln::cl_RA& n) {
	return cln::numerator(n);
}

inline const cln::cl_I getDenom(const cln::cl_RA& n) {
	return cln::denominator(n);
}

inline bool isInteger(const cln::cl_I&) {
	return true;
}
inline bool isInteger(const cln::cl_RA& n) {
	return getDenom(n) == (cln::cl_I)(1);
}

/**
 * Conversion functions
 * 
 * The following function convert types to other types.
 */
inline double toDouble(const cln::cl_RA& n) {
	return cln::double_approx(n);
}
inline double toDouble(const cln::cl_I& n) {
	return cln::double_approx(n);
}

template<typename Integer>
inline Integer toInt(const cln::cl_I& n);

template<>
inline int toInt<int>(const cln::cl_I& n) {
    assert(n <= std::numeric_limits<int>::max());
    assert(n >= std::numeric_limits<int>::min());
    return cln::cl_I_to_int(n);
}

template<>
inline unsigned toInt<unsigned>(const cln::cl_I& n) {
    assert(n <= std::numeric_limits<unsigned>::max());
    return cln::cl_I_to_uint(n);
}

template<>
inline long int toInt<long int>(const cln::cl_I& n) {
    assert(n <= std::numeric_limits<long int>::max());
    assert(n >= std::numeric_limits<long int>::min());
    return cln::cl_I_to_long(n);
}

inline cln::cl_I toInt(const cln::cl_RA& n) {
	assert(isInteger(n));
	return getNum(n);
}

inline cln::cl_LF toLF(const cln::cl_RA& n) {
	return cln::cl_R_to_LF(n, std::max(cln::integer_length(cln::numerator(n)), cln::integer_length(cln::denominator(n))));
}

template<typename T>
inline T rationalize(double n) {
	return cln::rationalize(cln::cl_R(n));
}
template<>
inline cln::cl_RA rationalize<cln::cl_RA>(double n) {
	switch (std::fpclassify(n)) {
		case FP_NORMAL: // normalized are fully supported
			return cln::rationalize(cln::cl_R(n));
		case FP_SUBNORMAL: // subnormals result in underflows, hence we just return zero.
			return 0;
		case FP_ZERO:
			return 0;
		case FP_NAN: // NaN and infinite are not supported
		case FP_INFINITE:
			assert(false);
			break;
	}
	return 0;
}

/**
 * Basic Operators
 * 
 * The following functions implement simple operations on the given numbers.
 */
inline cln::cl_I abs(const cln::cl_I& n) {
	return cln::abs(n);
}

inline cln::cl_RA abs(const cln::cl_RA& n) {
	return cln::abs(n);
}

inline cln::cl_I floor(const cln::cl_RA& n) {
	return cln::floor1(n);
}
inline cln::cl_I floor(const cln::cl_I& n) {
	return n;
}

inline cln::cl_I ceil(const cln::cl_RA& n) {
	return cln::ceiling1(n);
}
inline cln::cl_I ceil(const cln::cl_I& n) {
	return n;
}

inline cln::cl_I gcd(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::gcd(a,b);
}

inline cln::cl_I& gcd_here(cln::cl_I& a, const cln::cl_I& b) {
    a = cln::gcd(a,b);
	return a;
}

inline cln::cl_RA& gcd_here(cln::cl_RA& a, const cln::cl_RA& b) {
    assert( carl::isInteger( a ) );
    assert( carl::isInteger( b ) );
	a = cln::gcd(carl::getNum(a),carl::getNum(b));
	return a;
}

inline cln::cl_RA gcd(const cln::cl_RA& a, const cln::cl_RA& b) {
    assert( carl::isInteger( a ) );
    assert( carl::isInteger( b ) );
	return cln::gcd(carl::getNum(a),carl::getNum(b));
}

inline cln::cl_I lcm(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::lcm(a,b);
}

inline cln::cl_RA lcm(const cln::cl_RA& a, const cln::cl_RA& b) {
    assert( carl::isInteger( a ) );
    assert( carl::isInteger( b ) );
	return cln::lcm(carl::getNum(a),carl::getNum(b));
}

inline cln::cl_RA pow(const cln::cl_RA& n, unsigned e) {
	return cln::expt(n, (int)e);
}

inline std::pair<cln::cl_RA, cln::cl_RA> sqrt(const cln::cl_RA& a) {
    assert( a >= 0 );
    cln::cl_R root = cln::sqrt(toLF(a));
    cln::cl_RA rroot = cln::rationalize(root);
    if( rroot == root ) // the result of the sqrt operation is a rational and thus an exact solution -> return a point-Interval
    {
        return std::make_pair(rroot, rroot);
    }
    else // we need to find the second bound of the overapprox. - the first is given by the rationalized result.
    {
		// Check if root^2 > a
        if( cln::expt_pos(rroot,2) > a ) // we need to find the lower bound
        {
            cln::cl_R lower = cln::sqrt(toLF(a-rroot));
            cln::cl_RA rlower = cln::rationalize(lower);
            if( rlower == lower )
            {
                return std::make_pair(rlower, rroot);
            }
            else
            {
                cln::cl_I num = cln::numerator(rlower);
                cln::cl_I den = cln::denominator(rlower);
                --num;
                return std::make_pair( num/den, rroot );
            }
        }
        else // we need to find the upper bound
        {
            cln::cl_R upper = cln::sqrt(toLF(a+rroot));
            cln::cl_RA rupper = cln::rationalize(upper);
            if( rupper == upper )
            {
                return std::make_pair(rroot, rupper);
            }
            else
            {
                cln::cl_I num = cln::numerator(rupper);
                cln::cl_I den = cln::denominator(rupper);
                ++num;
                return std::make_pair(rroot, num/den );
            }
        }
    }
}

/**
 * Compute square root in a fast but less precise way.
 * Use cln::sqrt() to obtain an approximation. If the result is rational, i.e. the result is exact, use this result.
 * Otherwise use the nearest integers as bounds on the square root.
 * @param a Some number.
 * @return [x,x] if sqrt(a) = x is rational, otherwise [y,z] for y,z integer and y < sqrt(a) < z. 
 */
inline std::pair<cln::cl_RA, cln::cl_RA> sqrt_fast(const cln::cl_RA& a) {
	assert(a >= 0);
	cln::cl_R tmp = cln::sqrt(toLF(a));
	cln::cl_RA root = cln::rationalize(tmp);
	if(root == tmp) {
		// root is a cln::cl_RA
		return std::make_pair(root, root);
	} else {
		// root is a cln::cl_LF. In this case, it is not integer and we can assume that the surrounding integers contain the actual root.
		cln::cl_I lower = carl::floor(root);
		cln::cl_I upper = carl::ceil(root);
		assert(cln::expt_pos(lower,2) < a);
		assert(cln::expt_pos(upper,2) > a);
		return std::make_pair(lower, upper);
    }
}

inline cln::cl_I mod(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::mod(a, b);
}

inline cln::cl_RA div(const cln::cl_RA& a, const cln::cl_RA& b) {
	return (a / b);
}

inline cln::cl_I div(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::exquo(a, b);
}

inline cln::cl_RA& div_here(cln::cl_RA& a, const cln::cl_RA& b) {
    a /= b;
	return a;
}

inline cln::cl_I& div_here(cln::cl_I& a, const cln::cl_I& b) {
	a = cln::exquo(a, b);
    return a;
}

inline cln::cl_RA quotient(const cln::cl_RA& a, const cln::cl_RA& b)
{
	return a / b;
}
inline cln::cl_I quotient(const cln::cl_I& a, const cln::cl_I& b)
{
	return div(a,b);
}
//
inline cln::cl_I operator/(const cln::cl_I& a, const cln::cl_I& b)
{
	return div(a,b);
}

}
