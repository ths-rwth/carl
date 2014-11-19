/** 
 * @file:   numbers/operations.h
 * @author: Sebastian Junges
 *
 * @since November 6, 2014
 */

#pragma once

#include "../util/SFINAE.h"

namespace carl
{
	template<typename T, EnableIf<has_isZero<T>>>
	bool isZero(const T& t) 
	{
		return t.isZero();
	}
	
	template<typename T, DisableIf<has_isZero<T>>>
	bool isZero(const T& t)
	{
		return t == 0;
	}
	
	template<typename T, EnableIf<has_isOne<T>>>
	bool isOne(const T& t) 
	{
		return t.isOne();
	}
	
	template<typename T, DisableIf<has_isOne<T>>>
	bool isOne(const T& t) 
	{
		return t.isOne();
	}
	
	
	
	
}