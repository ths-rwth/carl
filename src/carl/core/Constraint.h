/** 
 * @file   Constraint.h
 * @author Sebastian Junges
 * @ingroup constraints
 * @since April 4, 2014
 */

#pragma once

namespace carl
{

template<typename LhsType>
class Constraint
{
	public:
	Constraint(const LhsType& lhs, CompareRelation rel) : mLhs(lhs), mRelation(rel)
	{}
	
	LhsType mLhs;
	CompareRelation mRelation;
};

}