#pragma once

#include <carl-common/meta/SFINAE.h>
#include "../../carl/constraint/BasicConstraint.h"
#include "../../carl/poly/umvpoly/functions/Substitution.h"


namespace carl {

/* operations for rationals */

template<typename Number, typename = std::enable_if_t<is_number<Number>::value>>
const Number& branching_point(const Number& n) {
	return n;
}

// TODO move:
template<typename Number, typename = std::enable_if_t<is_number<Number>::value>>
Number evaluate(const MultivariatePolynomial<Number>& p, const std::map<Variable, Number>& m) {
	MultivariatePolynomial<Number> res = p;
	for (const auto& e: m) {
		carl::substitute_inplace(res, e.first, MultivariatePolynomial<Number>(e.second));
	}
	assert(res.isConstant());
	return res.constantPart();
}

// TODO move:
template<typename Number, typename Poly, typename = std::enable_if_t<is_number<Number>::value>>
bool evaluate(const BasicConstraint<Poly>& c, const std::map<Variable, Number>& m) {
	auto res = evaluate(c.lhs(), m);
	return evaluate(res, c.relation());
}

template<typename Number, typename = std::enable_if_t<is_number<Number>::value>>
Number sample_above(const Number& n) {
	return carl::floor(n) + 1;
}
template<typename Number, typename = std::enable_if_t<is_number<Number>::value>>
Number sample_below(const Number& n) {
	return carl::ceil(n) - 1;
}
template<typename Number, typename = std::enable_if_t<is_number<Number>::value>>
Number sample_between(const Number& lower, const Number& upper) {
	return sample(Interval<Number>(lower, upper), false);
}

}