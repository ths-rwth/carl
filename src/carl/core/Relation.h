/** 
 * @file Relation.h
 * @ingroup constraints
 * @author Sebastian Junges
 */

#pragma once

#include "logging.h"
#include "Sign.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>

namespace carl {

enum class Relation { EQ = 0, NEQ = 1, LESS = 2, LEQ = 4, GREATER = 3, GEQ = 5 };

inline std::ostream& operator<<(std::ostream& os, const Relation& r) {
	switch (r) {
		case Relation::EQ:		os << "="; break;
		case Relation::NEQ:		os << "!="; break;
		case Relation::LESS:	os << "<"; break;
		case Relation::LEQ:		os << "<="; break;
		case Relation::GREATER:	os << ">"; break;
		case Relation::GEQ:		os << ">="; break;
		default:
			CARL_LOG_ERROR("carl.relation", "Invalid relation " << std::underlying_type_t<Relation>(r));
			assert(false && "Invalid relation");
	}
	return os;
}

inline Relation inverse(Relation r)
{
	switch (r) {
		case Relation::EQ:		return Relation::NEQ;
		case Relation::NEQ:		return Relation::EQ;
		case Relation::LESS:	return Relation::GEQ;
		case Relation::LEQ:		return Relation::GREATER;
		case Relation::GREATER:	return Relation::LEQ;
		case Relation::GEQ:		return Relation::LESS;
		default:
			CARL_LOG_ERROR("carl.relation", "Invalid relation " << std::underlying_type_t<Relation>(r));
			assert(false && "Invalid relation");
	}
	return Relation::EQ;
}

inline std::string toString(Relation r) {
	std::stringstream ss;
	ss << r;
	return ss.str();
}

inline bool isStrict(Relation r) {
	return r == Relation::LESS || r == Relation::GREATER || r == Relation::NEQ;
}
inline bool isWeak(Relation r) {
	return !isStrict(r);
}

inline bool evaluate(Sign s, Relation r) {
	switch (s) {
		case Sign::NEGATIVE:
			return r == Relation::NEQ || r == Relation::LESS || r == Relation::LEQ;
		case Sign::ZERO:
			return r == Relation::EQ || r == Relation::LEQ || r == Relation::GEQ;
		case Sign::POSITIVE:
			return r == Relation::NEQ || r == Relation::GREATER || r == Relation::GEQ;
		default:
			CARL_LOG_ERROR("carl.relation", "Evaluating invalid sign " << std::underlying_type_t<Sign>(s));
			assert(false && "Invalid sign");
	}
	return false;
}

template<typename T>
inline bool evaluate(const T& t, Relation r) {
	return evaluate(sgn(t), r);
}

template<typename T1, typename T2>
inline bool evaluate(const T1& lhs, Relation r, const T2& rhs) {
	return evaluate(sgn(lhs - rhs), r);
}
	
}

namespace std {

template<>
struct hash<carl::Relation> {
	std::size_t operator()(const carl::Relation& rel) const {
		return std::size_t(rel);
	}
};

}
