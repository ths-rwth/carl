/** 
 * @file CompareRelation.h
 * @ingroup constraints
 * @author Sebastian Junges
 */

#pragma once
#include <iostream>
#include <memory>

namespace carl {

enum class CompareRelation: unsigned { EQ, NEQ, LT, LEQ, GT, GEQ };
	
inline std::ostream& operator<<(std::ostream& os, const CompareRelation& r) {
	switch (r) {
		case CompareRelation::EQ:	os << "="; break;
		case CompareRelation::NEQ:	os << "<>"; break;
		case CompareRelation::LT:	os << "<"; break;
		case CompareRelation::LEQ:	os << "<="; break;
		case CompareRelation::GT:	os << ">"; break;
		case CompareRelation::GEQ:	os << ">="; break;
	}
	return os;
}

inline std::string toString(CompareRelation r) {
	std::stringstream ss;
	ss << r;
	return ss.str();
}

inline bool relationIsStrict(CompareRelation r) {
	return r == CompareRelation::LT || r == CompareRelation::GT || r == CompareRelation::NEQ;
}
	
}

namespace std {

template<>
struct hash<carl::CompareRelation> {
	std::size_t operator()(const carl::CompareRelation& rel) const {
		return std::size_t(rel);
	}
};

}