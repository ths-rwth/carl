/** 
 * @file   operations_cln.h
 * @ingroup cln
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * @author Sebastian Junges
 * 
 * @warning This file should never be included directly but only via operations.h
 * 
 */

#pragma once

#include <cassert>
#include <climits>

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
    assert(n <= INT_MAX);
    return cln::cl_I_to_int(n);
}
template<>
inline unsigned toInt<unsigned>(const cln::cl_I& n) {
    assert(n <= UINT_MAX);
    return cln::cl_I_to_uint(n);
}

template<typename T>
inline T rationalize(double n) {
	return cln::rationalize(cln::cl_R(n));
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

inline cln::cl_I lcm(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::lcm(a,b);
}

inline cln::cl_RA pow(const cln::cl_RA& n, unsigned e) {
	return cln::expt(n, (int)e);
}

inline std::pair<cln::cl_RA, cln::cl_RA> sqrt(const cln::cl_RA& a) {
    assert( a >= 0 );
    cln::cl_R root = cln::sqrt(a);
    cln::cl_RA rroot = cln::rationalize(root);
    if( rroot == root ) // the result of the sqrt operation is a rational and thus an exact solution -> return a point-Interval
    {
        return std::make_pair(rroot, rroot);
    }
    else // we need to find the second bound of the overapprox. - the first is given by the rationalized result.
    {
        if( cln::expt_pos(rroot,2) > a ) // we need to find the lower bound
        {
            cln::cl_R lower = cln::sqrt(a-rroot);
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
            cln::cl_R upper = cln::sqrt(a+rroot);
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

inline cln::cl_I mod(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::mod(a, b);
}

inline cln::cl_I div(const cln::cl_I& a, const cln::cl_I& b) {
	return cln::floor1(a / b);
}

}