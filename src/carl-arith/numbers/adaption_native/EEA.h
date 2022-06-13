/** 
 * @file:   EEA.h
 * @author: Sebastian Junges
 *
 * @since October 21, 2013
 */

#pragma once

#include <cassert>
#include <utility>

namespace carl {
/**
 * Extended euclidean algorithm for numbers.
 */
template<typename IntegerType>
struct EEA {

	static std::pair<IntegerType, IntegerType> calculate(const IntegerType& a, const IntegerType& b) {
		IntegerType s;
		IntegerType t;
		if (a >= 0 && b >= 0) {
			calculate_recursive(a, b, s, t);
		} else if (a < 0 && b >= 0) {
			calculate_recursive(-a, b, s, t);
		} else if (a >= 0 && b < 0) {
			calculate_recursive(a, -b, s, t);
		} else {
			calculate_recursive(-a, -b, s, t);
		}

		if (a < 0) {
			s = -s;
		}
		if (b < 0) {
			t = -t;
		}
		return std::make_pair(s, t);
	}
	/// \todo a iterative implementation might be faster
	static void calculate_recursive(const IntegerType& a, const IntegerType& b, IntegerType& s, IntegerType& t) {
		assert(a >= 0 && b >= 0);
		if (b == 0) {
			s = 1;
			t = 0;
		} else {
			IntegerType quotient(0);
			IntegerType remainder(0);
			IntegerType tmp(0);
			// quotient, remainder = divide(a,b)
			divide(a, b, quotient, remainder);
			calculate_recursive(b, remainder, s, tmp);
			t = s - quotient * tmp;
			s = tmp;
		}
	}
};
} // namespace carl