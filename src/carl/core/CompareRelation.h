/** 
 * @file   Relation.h
 * @ingroup constraints
 * @author Sebastian Junges
 *
 */

#pragma once
#include <string>
#include <iostream>

namespace carl
{
	enum class CompareRelation { EQ, NEQ, LT, LE, GT, GE };
	
	bool relationIsStrict(CompareRelation r);
	std::string toString(const CompareRelation& cr);
	std::ostream& operator<<(std::ostream&, CompareRelation);
	
}