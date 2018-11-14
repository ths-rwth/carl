#pragma once

#include "Interval.h"

#include <cassert>

namespace carl {

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> exp(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::exp(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void exp_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::exp(i.rContent()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> log(const Interval<Number>& i) {
	assert(i.isConsistent() && i.isSemiPositive());
	return Interval<Number>(boost::numeric::log(i.rContent()), i.lowerBoundType(), i.upperBoundType());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void log_assign(Interval<Number>& i) {
	assert(i.isConsistent() && i.isSemiPositive());
	i.set(boost::numeric::log(i.rContent()));
}

}