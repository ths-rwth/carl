/* 
 * File:   operations_native.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * This file should never be included directly but only via operations.h
 */

#pragma once

#include <cmath>

#include "typetraits.h"

namespace carl {

/**
 * Informational functions
 * 
 * The following functions return informations about the given numbers.
 */

/**
 * Conversion functions
 * 
 * The following function convert types to other types.
 */

inline double toDouble(const int& n) {
	return double(n);
}

/**
 * Basic Operators
 * 
 * The following functions implement simple operations on the given numbers.
 */
inline unsigned floor(const double& n) {
	return (unsigned)std::floor(n);
}
inline unsigned ceil(const double& n) {
	return (unsigned)std::ceil(n);
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