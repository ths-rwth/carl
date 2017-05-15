/** 
 * @file core/SimpeConstraint.h
 * @author Sebastian Junges
 * @ingroup constraints
 * @since April 4, 2014
 */

#pragma once
#include "Relation.h"
namespace carl
{

template<typename LhsType>
class SimpleConstraint
{
	public:
		SimpleConstraint(bool v) : mLhs(v ? 0 : 1), mRelation(Relation::EQ) {}
		SimpleConstraint(const LhsType& lhs, Relation rel) : mLhs(lhs), mRelation(rel)
		{}
	
		const LhsType& lhs() const {return mLhs;}
		const Relation& rel() const {return mRelation;}
		
		bool isTrivialTrue() const  {
			if(mLhs.isConstant())
			{
				auto cp = mLhs.constantPart();
				if(cp == 0) {
					return isWeak(mRelation);
				} else if(cp > 0) {
					return mRelation == Relation::GEQ || mRelation == Relation::GREATER || mRelation == Relation::NEQ;
				} else {
					assert(cp < 0);
					return mRelation == Relation::LEQ || mRelation == Relation::LESS || mRelation == Relation::NEQ;
				}
			}
			return false;
		}
		
		bool isTrivialFalse() const {
			if(mLhs.isConstant())
			{
				auto cp = mLhs.constantPart();
				if(cp == 0) {
					return isStrict(mRelation);
				} else if(cp > 0) {
					return mRelation == Relation::LEQ || mRelation == Relation::LESS || mRelation == Relation::EQ;
				} else {
					assert(cp < 0);
					return mRelation == Relation::GEQ || mRelation == Relation::GREATER || mRelation == Relation::EQ;
				}
			}
			return false;
		}
				
	private:
		LhsType mLhs;
		Relation mRelation;
};

template<typename LhsT>
bool operator==(const SimpleConstraint<LhsT>& lhs, const SimpleConstraint<LhsT>& rhs)
{
	return lhs.rel() == rhs.rel() && lhs.lhs() == rhs.lhs();
}

template<typename LhsT>
bool operator!=(const SimpleConstraint<LhsT>& lhs, const SimpleConstraint<LhsT>& rhs)
{
	return !(lhs == rhs);
}

template<typename LhsT>
std::ostream& operator<<(std::ostream& os, const SimpleConstraint<LhsT>& rhs)
{
	return os << rhs.lhs() << " " << rhs.rel() << " 0";
}

template<typename LhsT>
std::string to_string(const SimpleConstraint<LhsT>& constraint, bool pretty = false) {
	if (pretty) {
		std::stringstream strstr;
		strstr << constraint.lhs() << " " << constraint.rel() << " 0";
		return strstr.str();
	} else {
		std::stringstream strstr;
		strstr << constraint.lhs() << "," << constraint.rel();
		return strstr.str();
	}
}


}


namespace std {

template<typename LhsType>
struct hash<carl::SimpleConstraint<LhsType>> 
{
	std::size_t operator()(const carl::SimpleConstraint<LhsType>& c) const {
		std::hash<LhsType> h1;
		std::hash<carl::Relation> h2;
		return h1(c.lhs()) << 2 | h2(c.rel());
	}
};

}
