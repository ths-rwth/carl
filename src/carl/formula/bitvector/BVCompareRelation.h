/** 
 * @file BVCompareRelation.h
 * @author Andreas Krueger <andreas.krueger@rwth-aachen.de>
 */

#pragma once
#include <iostream>

namespace carl {

enum class BVCompareRelation: unsigned
{
	EQ,  // =
	NEQ, // <>
	ULT, // unsigned <
	ULE, // unsigned <=
	UGT, // unsigned >
	UGE, // unsigned >=
	SLT, // signed <
	SLE, // signed <=
	SGT, // signed >
	SGE  // signed >=
};
	
inline std::string toString(BVCompareRelation r)
{
	switch (r) {
		case BVCompareRelation::EQ:     return "=";
		case BVCompareRelation::NEQ:    return "<>";
		case BVCompareRelation::ULT:    return "bvult";
		case BVCompareRelation::ULE:    return "bvule";
		case BVCompareRelation::UGT:    return "bvugt";
		case BVCompareRelation::UGE:    return "bvuge";
		case BVCompareRelation::SLT:    return "bvslt";
		case BVCompareRelation::SLE:    return "bvsle";
		case BVCompareRelation::SGT:    return "bvsgt";
		case BVCompareRelation::SGE:    return "bvsge";
	}
	assert(false);
	return "";
}

inline std::ostream& operator<<(std::ostream& os, const BVCompareRelation& r)
{
	return (os << toString(r));
}

inline BVCompareRelation inverse(BVCompareRelation c)
{
	switch (c) {
		case BVCompareRelation::EQ:     return BVCompareRelation::NEQ;
		case BVCompareRelation::NEQ:    return BVCompareRelation::EQ;
		case BVCompareRelation::ULT:    return BVCompareRelation::UGE;
		case BVCompareRelation::ULE:    return BVCompareRelation::UGT;
		case BVCompareRelation::UGT:    return BVCompareRelation::ULE;
		case BVCompareRelation::UGE:    return BVCompareRelation::ULT;
		case BVCompareRelation::SLT:    return BVCompareRelation::SGE;
		case BVCompareRelation::SLE:    return BVCompareRelation::SGT;
		case BVCompareRelation::SGT:    return BVCompareRelation::SLE;
		case BVCompareRelation::SGE:    return BVCompareRelation::SLT;
	}
	assert(false);
	return BVCompareRelation::EQ;
}

inline bool relationIsStrict(BVCompareRelation r) {
	return r == BVCompareRelation::ULT ||
	       r == BVCompareRelation::UGT ||
	       r == BVCompareRelation::SLT ||
	       r == BVCompareRelation::UGT ||
	       r == BVCompareRelation::NEQ;
}

inline bool relationIsSigned(BVCompareRelation r) {
	return r == BVCompareRelation::SLT ||
	       r == BVCompareRelation::SLE ||
	       r == BVCompareRelation::SGT ||
	       r == BVCompareRelation::SGE;
}
	
} // end namespace carl

namespace std {

template<>
struct hash<carl::BVCompareRelation> {
	std::size_t operator()(const carl::BVCompareRelation& rel) const {
		return std::size_t(rel);
	}
};

} // end namespace std
