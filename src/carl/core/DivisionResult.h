/** 
 * @file:   DivisionResult.h
 * @author: Sebastian Junges
 *
 * @since October 15, 2013
 */

#pragma once

template<typename Type>
struct DivisionResult
{
	DivisionResult()
	{
		
	}
	
	DivisionResult(const Type& q, const Type& r) :
	quotient(q), remainder(r)
	{
		
	}
	
	Type quotient;
	Type remainder;
};