/*
 * File:   operations_native.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 *
 * This file should never be included directly but only via operations.h
 */

#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include <cassert>
#include <cmath>
#include <limits>

namespace carl {

/**
 * Informational functions
 *
 * The following functions return informations about the given numbers.
 */

inline bool isZero(const int& n) {
	return n == 0;
}
inline bool isOne(const int& n) {
	return n == 1;
}
inline bool isPositive(const int& n) {
	return n > 0;
}
inline bool isNegative(const int& n) {
	return n < 0;
}

inline bool isNumber(const double& d) {
	return (d == d) && !std::isinf(d);
}

inline bool isInteger(const double& d) {
	double tmp;
	return std::modf(d, &tmp) == 0.0;
}

inline bool isInteger(const int& /*unused*/) {
	return true;
}

inline bool isNegative(const double& d) {
	return d < 0;
}

inline bool isPositive(const double& d) {
	return d > 0;
}

inline std::size_t bitsize(unsigned /*unused*/) {
	return sizeof(unsigned) * 8;
}

template<typename C>
inline void reserve(std::size_t /*unused*/) {
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

template<typename Integer>
inline Integer toInt(const double& n);

template<>
inline int toInt<int>(const double& n) {
    return int(n);
}

template<>
inline long toInt<long>(const double& n) {
    return long(n);
}

template<>
inline std::size_t toInt<std::size_t>(const double& n) {
    return size_t(n);
}

template<>
inline double rationalize(double n) { return n; }

template<typename T>
inline typename std::enable_if<std::is_arithmetic<T>::value(), std::string>::type toString(const T& n) {
	return std::to_string(n);
}

inline std::string toString(const double& n) {
	return std::to_string(n);
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

inline int remainder(const int& n, const int& m) {
	return n % m;
}
inline int div(const int& n, const int& m) {
	assert(n % m == 0);
	return n / m;
}
inline int quotient(const int& n, const int& m) {
	return n / m;
}
inline void divide(const int& dividend, const int& divisor, int& quo, int& rem) {
	quo = quotient(dividend, divisor);
	rem = remainder(dividend, divisor);
}

inline double sin(double in) {
	return std::sin(in);
}

inline double cos(double in) {
	return std::cos(in);
}

inline double acos(double in) {
	return std::acos(in);
}

inline double sqrt(double in) {
	return std::sqrt(in);
}

inline double pow(double in, size_t exp) {
	return std::pow(in,exp);
}

inline double log(double in) {
	return std::log(in);
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
	static_assert(std::is_fundamental<Number>::value, "Only works on native types.");
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
