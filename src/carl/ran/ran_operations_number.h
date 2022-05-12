#pragma once

#include <carl-common/meta/SFINAE.h>
#include <carl/constraint/BasicConstraint.h>
#include <carl/poly/umvpoly/functions/Evaluation.h>
#include <carl/constraint/Evaluation.h>

namespace carl {

/* operations for rationals */

template<typename Number, typename = std::enable_if_t<is_number<Number>::value>>
const Number& branching_point(const Number& n) {
	return n;
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