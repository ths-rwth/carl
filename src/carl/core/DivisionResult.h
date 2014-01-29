/** 
 * @file   DivisionResult.h
 * @author Sebastian Junges
 *
 */

#pragma once

namespace carl
{
	
/**
 * A strongly typed pair encoding the result of a division, 
 * being a quotient and a remainder.
 */	
template<typename Type>
struct DivisionResult
{
public:
	DivisionResult(const Type& q, const Type& r) :
	quotient(q), remainder(r)
	{}
	Type quotient;
	Type remainder;
};
}