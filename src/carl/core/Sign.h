/** 
 * @file Sign.h
 * @author Sebastian Junges
 *
 * @since August 23, 2013
 */

#pragma once

#include "logging.h"

#include <functional>
#include <iostream>

namespace carl {
/**
 * This class represents the sign of a number \f$n\f$.
 */
enum class Sign: int {
	/// Indicates that \f$n < 0\f$.
	NEGATIVE = -1,
	/// Indicates that \f$n = 0\f$.
	ZERO = 0,
	/// Indicates that \f$n > 0\f$.
	POSITIVE = 1
};

inline std::ostream& operator<<(std::ostream& os, const Sign& sign) {
	switch (sign) {
		case Sign::NEGATIVE:	os << "NEGATIVE"; break;
		case Sign::ZERO:		os << "ZERO"; break;
		case Sign::POSITIVE:	os << "POSITIVE"; break;
		default:
			CARL_LOG_ERROR("carl.sign", "Invalid sign " << std::underlying_type_t<Sign>(s));
			assert(false && "Invalid sign");
	}
	return os;
}

/**
 * Obtain the sign of the given number.
 * This method relies on the comparison operators for the type of the given number.
 * @param n Number
 * @return Sign of n
 */
template<typename Number>
Sign sgn(const Number& n) {
	if (n < Number(0)) return Sign::NEGATIVE;
	if (n > Number(0)) return Sign::POSITIVE;
	return Sign::ZERO;
}

/**
 * Counts the number of sign variations in the given object range.
 *
 * The function accepts an range of Sign objects.
 * @param begin Start of object range.
 * @param end End of object range.
 * @return Sign variations of objects.
 */
template<typename InputIterator>
std::size_t signVariations(InputIterator begin, InputIterator end) {
	while ((*begin == Sign::ZERO) && (begin != end)) begin++;

	std::size_t changes = 0;
	for (Sign last = *begin; begin != end; begin++) {
		if (*begin == Sign::ZERO) continue;
		if (*begin != last) changes++;
		last = *begin;
	}
	return changes;
}

/**
 * Counts the number of sign variations in the given object range.
 *
 * The function accepts an object range and an additional function f.
 * If the objects are not of type Sign, the function f can be used to convert the objects to a Sign on the fly.
 * As for the number of sign variations in the evaluations of polynomials <code>p</code> at a position <code>x</code>, this might look like this:
 * <code>
 * signVariations(p.begin(), p.end(), [&x](const Polynomial& p){ return sgn(p.evaluate(x)); });
 * </code>
 * @param begin Start of object range.
 * @param end End of object range.
 * @param f Function object to convert objects to Sign.
 * @return Sign variations of objects.
 */
template<typename InputIterator, typename Function>
std::size_t signVariations(InputIterator begin, InputIterator end, const Function& f) {
	while ((begin != end) && (f(*begin) == Sign::ZERO)) begin++;
	if (begin == end) return 0;

	std::size_t changes = 0;
	for (Sign last = f(*begin); begin != end; begin++) {
		if (f(*begin) == Sign::ZERO) continue;
		if (f(*begin) != last) changes++;
		last = f(*begin);
	}
	return changes;
}

}
