/* 
 * File:   operations_native.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * This file should never be included directly but only via operations.h
 */

#pragma once

#include <cmath>
#include <limits>

namespace carl {

/**
 * Informational functions
 * 
 * The following functions return informations about the given numbers.
 */

inline bool isNumber(const double& d) {
	return (d == d) && !std::isinf(d);
}

inline std::size_t bitsize(unsigned) {
	return sizeof(unsigned) * 8;
}

template<typename C>
inline void reserve(size_t) {
}

/**
 * Conversion functions
 * 
 * The following function convert types to other types.
 */

inline double toDouble(const int& n) {
	return double(n);
}
inline double toDouble(const double& n) {
	return n;
}

/**
 * Basic Operators
 * 
 * The following functions implement simple operations on the given numbers.
 */
inline double floor(double n) {
	return std::floor(n);
}
inline double ceil(double n) {
	return std::ceil(n);
}
inline double abs(double n) {
	return std::abs(n);
}

/** Returns a down-rounded representation of the given numeric
 * @param o
 * @param overapproximate
 * @return double representation of o (underapprox) Note, that it can return the double INFINITY.
 */
template<typename Rational>
static double roundDown(const Rational& o, bool overapproximate = false) {
    typedef std::numeric_limits<double> limits;
    double result = carl::toDouble(o);
    if (result == -limits::infinity()) return result;
    if (result == limits::infinity()) return limits::max();
    // If the cln::cl_RA cannot be represented exactly by a double, round.
    if (overapproximate || carl::rationalize<Rational>(result) != o) {
            if (result == -limits::max()) return -limits::infinity();
            return std::nextafter(result, -limits::infinity());
    } else {
            return result;
    }
}

/** Returns a up-rounded representation of the given numeric
 * @param o
 * @param overapproximate
 * @return double representation of o (overapprox) Note, that it can return the double INFINITY.
 */
template<typename Rational>
static double roundUp(const Rational& o, bool overapproximate = false) {
    typedef std::numeric_limits<double> limits;
    double result = carl::toDouble(o);
    if (result == limits::infinity()) return result;
    if (result == -limits::infinity()) return -limits::max();
    // If the cln::cl_RA cannot be represented exactly by a double, round.
    if (overapproximate || carl::rationalize<Rational>(result) != o) {
            if (result == limits::max()) return limits::infinity();
            return std::nextafter(result, limits::infinity());
    } else {
            return result;
    }
}

inline unsigned long pow(const unsigned long& n, const unsigned exp) {
	unsigned long res = 1;
	unsigned long mult = n;
	unsigned e = exp;
	while (e > 0) {
		if (e & 1) res *= mult;
		mult *= mult;
		e /= 2;
	}
	return res;
}

inline long mod(const long& n, const long& m) {
	return n % m;
}
inline unsigned long mod(const unsigned long& n, const unsigned long& m) {
	return n % m;
}
inline unsigned mod(const unsigned& n, const unsigned& m) {
	return n % m;
}
inline int mod(const int& n, const int& m) {
	return n % m;
}

/**
 * Returns the highest power of two below n.
 *
 * Can also be seen as the highest bit set in n.
 * @param n
 * @return
 */
template<typename Number>
inline Number highestPower(const Number& n) {
	unsigned iterations = 0;
	// Number has 2^k Bits, we do k iterations
	if (sizeof(Number) == 2) iterations = 4;
	else if (sizeof(Number) == 4) iterations = 5;
	else if (sizeof(Number) == 8) iterations = 6;
	assert(iterations > 0);

	Number res = n;
	for (unsigned i = 0; i < iterations; i++) {
		res |= res >> (1 << i);
	}
	res -= res >> 1;
	return res;
}

}