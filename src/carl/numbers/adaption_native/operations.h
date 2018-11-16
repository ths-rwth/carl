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
#include <type_traits>

namespace carl {

/**
 * Informational functions
 *
 * The following functions return informations about the given numbers.
 */

template<typename T, EnableIf<std::is_fundamental<T>> = dummy>
inline bool isZero(T n) {
	return n == 0;
}
inline bool isZero(double n) {
	return std::fpclassify(n) == FP_ZERO;
}
template<typename T, EnableIf<std::is_fundamental<T>> = dummy>
inline bool isOne(T n) {
	return n == 1;
}
template<typename T, EnableIf<std::is_fundamental<T>> = dummy>
inline bool isPositive(T n) {
	return n > 0;
}
template<typename T, EnableIf<std::is_fundamental<T>> = dummy>
inline bool isNegative(T n) {
	return n < 0;
}

inline bool isNaN(double d) {
	if (std::is_same<decltype(std::isnan(d)), bool>::value) {
		return std::isnan(d);
	} else if (std::is_same<decltype(std::isnan(d)), int>::value) {
		return std::isnan(d) != 0;
	}
}
inline bool isInf(double d) {
	if (std::is_same<decltype(std::isinf(d)), bool>::value) {
		return std::isinf(d);
	} else if (std::is_same<decltype(std::isinf(d)), int>::value) {
		return std::isinf(d) != 0;
	}
}

inline bool isNumber(double d) {
	return !isNaN(d) && !isInf(d);
}

inline bool isInteger(double d) {
	double tmp;
	return std::fpclassify(std::modf(d, &tmp)) == FP_ZERO;
}

inline bool isInteger(sint /*unused*/) {
	return true;
}

inline bool isNegative(double d) {
	return d < 0;
}

inline bool isPositive(double d) {
	return d > 0;
}

inline std::size_t bitsize(unsigned /*unused*/) {
	return sizeof(unsigned) * 8;
}

/**
 * Conversion functions
 *
 * The following function convert types to other types.
 */

inline double toDouble(sint n) {
	return double(n);
}
inline double toDouble(double n) {
	return n;
}

template<typename Integer>
inline Integer toInt(double n);

template<>
inline sint toInt<sint>(double n) {
    return sint(n);
}

template<>
inline uint toInt<uint>(double n) {
    return uint(n);
}

template<>
inline double rationalize(double n) { return n; }

template<typename T>
inline typename std::enable_if<std::is_arithmetic<typename remove_all<T>::type>::value, std::string>::type toString(const T& n, bool /*unused*/) {
	return std::to_string(n);
}
//inline std::string toString(sint n, bool /*unused*/) {
//	return std::to_string(n);
//}
//inline std::string toString(uint n, bool /*unused*/) {
//	return std::to_string(n);
//}
//inline std::string toString(double n, bool /*unused*/) {
//	return std::to_string(n);
//}

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

inline uint mod(uint n, uint m) {
	return n % m;
}
inline sint mod(sint n, sint m) {
	return n % m;
}

inline sint remainder(sint n, sint m) {
	return n % m;
}
inline sint div(sint n, sint m) {
	assert(n % m == 0);
	return n / m;
}
inline sint quotient(sint n, sint m) {
	return n / m;
}
inline void divide(sint dividend, sint divisor, sint& quo, sint& rem) {
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

inline std::pair<double,double> sqrt_safe(double in) {
	return std::make_pair(std::sqrt(in), std::sqrt(in));
}

inline double pow(double in, uint exp) {
	return std::pow(in,exp);
}

inline double log(double in) {
	return std::log(in);
}
inline double log10(double in) {
	return std::log10(in);
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
	uint iterations = 0;
	// Number has 2^k Bits, we do k iterations
	if (sizeof(Number) == 2) iterations = 4;
	else if (sizeof(Number) == 4) iterations = 5;
	else if (sizeof(Number) == 8) iterations = 6;
	assert(iterations > 0);

	Number res = n;
	for (uint i = 0; i < iterations; i++) {
		res |= res >> (1 << i);
	}
	res -= res >> 1;
	return res;
}


}
