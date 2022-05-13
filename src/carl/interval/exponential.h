#pragma once

#include "Interval.h"

#include <cassert>

namespace carl {

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> exp(const Interval<Number>& i) {
	assert(i.isConsistent());
	return Interval<Number>(boost::numeric::exp(i.content()), i.lower_bound_type(), i.upper_bound_type());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void exp_assign(Interval<Number>& i) {
	assert(i.isConsistent());
	i.set(boost::numeric::exp(i.content()));
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> log(const Interval<Number>& i) {
	assert(i.isConsistent() && i.is_semi_positive());
	return Interval<Number>(boost::numeric::log(i.content()), i.lower_bound_type(), i.upper_bound_type());
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void log_assign(Interval<Number>& i) {
	assert(i.isConsistent() && i.is_semi_positive());
	i.set(boost::numeric::log(i.content()));
}

}