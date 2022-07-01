#pragma once

#include <carl-common/meta/SFINAE.h>

#include <carl-arith/poly/umvpoly/functions/Substitution.h>
#include <carl-arith/core/Relation.h>

#include <functional>

namespace carl {

template<typename T>
struct is_ran_type : std::false_type {};

/* operators */

template<typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
inline bool is_zero(const RAN& n) {
	return n.is_zero();
}

template<typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
inline bool is_integer(const RAN& n) {
	return n.is_integral();
}

template<typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
inline RAN abs(const RAN& n) {
	return n.abs();
}

template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
Number is_root_of(const UnivariatePolynomial<Number>& p, const RAN& value) {
	return value.sgn(p) == Sign::ZERO;
}


/* comparison operators */

template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator==(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::EQ);
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator!=(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::NEQ);
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator<=(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::LEQ);
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator>=(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::GEQ);
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator<(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::LESS);
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator>(const RAN& lhs, const Number& rhs) {
	return compare(lhs, rhs, Relation::GREATER);
}

template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator==(const Number& lhs, const RAN& rhs) {
	return rhs == lhs;
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator!=(const Number& lhs, const RAN& rhs) {
	return rhs != lhs;
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator<=(const Number& lhs, const RAN& rhs) {
	return rhs >= lhs;
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator>=(const Number& lhs, const RAN& rhs) {
	return rhs <= lhs;
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator<(const Number& lhs, const RAN& rhs) {
	return rhs > lhs;
}
template<typename Number, typename RAN, typename = std::enable_if_t<is_ran_type<RAN>::value>>
bool operator>(const Number& lhs, const RAN& rhs) {
	return rhs < lhs;
}

template<typename RAN, EnableIf<is_ran_type<RAN>> = dummy>
bool operator==(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::EQ);
}
template<typename RAN, EnableIf<is_ran_type<RAN>> = dummy>
bool operator!=(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::NEQ);
}
template<typename RAN, EnableIf<is_ran_type<RAN>> = dummy>
bool operator<=(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::LEQ);
}
template<typename RAN, EnableIf<is_ran_type<RAN>> = dummy>
bool operator>=(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::GEQ);
}
template<typename RAN, EnableIf<is_ran_type<RAN>> = dummy>
bool operator<(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::LESS);
}
template<typename RAN, EnableIf<is_ran_type<RAN>> = dummy>
bool operator>(const RAN& lhs, const RAN& rhs) {
	return compare(lhs, rhs, Relation::GREATER);
}

}

/*
namespace std {

this does not work with clang:

template <typename First, typename... Others>
using first = First;

template<typename RAN>
struct hash<first<RAN, std::enable_if_t<carl::is_ran_type<RAN>::value>>> {
	std::size_t operator()(const RAN& n) const {
		return carl::hash_all(n.integer_below());
	}
};

neither this:

template <typename RAN>
struct hash {
    typename std::enable_if_t<carl::is_ran_type<RAN>::value, std::size_t>
    operator()(const RAN& n) const {
		return carl::hash_all(n.integer_below());
	}
};


solution: hash is defined for each algebraic number type indivdually

}
*/