/** 
 * @file   Constraint.h
 * @author Sebastian Junges
 * @ingroup constraints
 * @since April 4, 2014
 */

#pragma once
#include "CompareRelation.h"
namespace carl
{

template<typename LhsType>
class Constraint
{
	public:
	Constraint(const LhsType& lhs, CompareRelation rel) : mLhs(lhs), mRelation(rel)
	{}
	
	const LhsType& lhs() const {return mLhs;}
	const CompareRelation& rel() const {return mRelation;}
	private:
	LhsType mLhs;
	CompareRelation mRelation;
};

}