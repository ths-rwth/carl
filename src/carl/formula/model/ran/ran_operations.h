#pragma once

#include <carl/util/SFINAE.h>

#include <carl/core/polynomialfunctions/Substitution.h>
#include <carl/core/Relation.h>

namespace carl {

template<typename T>
struct is_ran : std::false_type {};

/* operators */

template<typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
inline bool isZero(const RAN& n) {
	return n.isZero();
}

template<typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
inline bool isInteger(const RAN& n) {
	return n.is_integral();
}

template<typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
inline RAN abs(const RAN& n) {
	return n.abs();
}

template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
Number is_root_of(const UnivariatePolynomial<Number>& p, const RAN& value) {
	return value.sgn(p) == Sign::ZERO;
}


/* comparison operators */

template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator==(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::EQ);
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator!=(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::NEQ);
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator<=(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::LEQ);
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator>=(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::GEQ);
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator<(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::LESS);
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator>(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::GREATER);
}

template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator==(const Number& lhs, const RAN& rhs) {
	return rhs == lhs;
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator!=(const Number& lhs, const RAN& rhs) {
	return rhs != lhs;
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator<=(const Number& lhs, const RAN& rhs) {
	return rhs >= lhs;
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator>=(const Number& lhs, const RAN& rhs) {
	return rhs <= lhs;
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator<(const Number& lhs, const RAN& rhs) {
	return rhs > lhs;
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran<RAN>::value>>
bool operator>(const Number& lhs, const RAN& rhs) {
	return rhs < lhs;
}

template<typename RAN, EnableIf<carl::is_ran<RAN>> = dummy>
bool operator==(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::EQ);
}
template<typename RAN, EnableIf<carl::is_ran<RAN>> = dummy>
bool operator!=(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::NEQ);
}
template<typename RAN, EnableIf<carl::is_ran<RAN>> = dummy>
bool operator<=(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::LEQ);
}
template<typename RAN, EnableIf<carl::is_ran<RAN>> = dummy>
bool operator>=(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::GEQ);
}
template<typename RAN, EnableIf<carl::is_ran<RAN>> = dummy>
bool operator<(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::LESS);
}
template<typename RAN, EnableIf<carl::is_ran<RAN>> = dummy>
bool operator>(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::GREATER);
}

}

namespace std {


template <typename First, typename... Others>
using first = First;

template<typename RAN>
struct hash<first<RAN, std::enable_if_t<carl::is_ran<RAN>::value>>> {
	std::size_t operator()(const RAN& n) const {
		return carl::hash_all(n.integer_below());
	}
};

}