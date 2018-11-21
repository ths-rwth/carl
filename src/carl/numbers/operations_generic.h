/** 
 * @file:   operations_generic.h
 * @author: Sebastian Junges
 *
 * @since November 6, 2014
 */

#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include "../util/SFINAE.h"
#include "constants.h"

namespace carl
{
	
	template<typename T>
	inline bool isZero(const T& t)
	{
		return t == 0;
	}
	
	template<typename T>
	inline bool isOne(const T& t) 
	{
		return t == 1;
	}
    
	template<typename T, EnableIf<has_isPositive<T>>>
	inline bool isPositive(const T& t) 
	{
		return t.isPositive();
	}
	
	template<typename T, DisableIf<has_isPositive<T>>>
	inline bool isPositive(const T& t)
	{
		return t > 0;
	}
	
	template<typename T, EnableIf<has_isNegative<T>>>
	inline bool isNegative(const T& t) 
	{
		return t.isNegative();
	}
	
	template<typename T, DisableIf<has_isNegative<T>>>
	inline bool isNegative(const T& t) 
	{
		return t < 0;
	}
	
	/**
	 * Implements a fast exponentiation on an arbitrary type T.
	 * To use carl::pow() on a type T, the following must be defined:
	 * - carl::constant_one<T>,
	 * - T::operator=(const T&) and
	 * - operator*(const T&, const T&).
	 * Alternatively, carl::pow() can be specialized for T explicitly.
	 * @param t A number.
	 * @param exp The exponent.
	 * @return `t` to the power of `exp`.
	 */
	template<typename T, DisableIf<is_interval<T>> = dummy>
	T pow(const T& basis, std::size_t exp) {
		T res = carl::constant_one<T>().get();
		T mult = basis;
		for (std::size_t e = exp; e > 0; e /= 2) {
			if (e & 1) res *= mult;
			mult *= mult;
		}
		return res;
	}

	/**
	 * Implements a fast exponentiation on an arbitrary type T.
	 * The result is stored in the given number.
	 * To use carl::pow_assign() on a type T, the following must be defined:
	 * - carl::constant_one<T>,
	 * - T::operator=(const T&) and
	 * - operator*(const T&, const T&).
	 * Alternatively, carl::pow() can be specialized for T explicitly.
	 * @param t A number.
	 * @param exp The exponent.
	 */
	template<typename T>
	void pow_assign(T& t, std::size_t exp) {
		T mult = t;
		t = carl::constant_one<T>().get();
		for (std::size_t e = exp; e > 0; e /= 2) {
			if (e & 1) t *= mult;
			mult *= mult;
		}
	}
	
}
