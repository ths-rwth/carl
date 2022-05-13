#pragma once

#include "Interval.h"

#include <cassert>


namespace carl {

template<typename Number, typename Integer>
Interval<Number> pow(const Interval<Number>& i, Integer exp) {
	assert(i.isConsistent());
	if (exp % 2 == 0) {
		if (i.is_infinite()) {
			return Interval<Number>(carl::constant_zero<Number>().get(), BoundType::WEAK, carl::constant_zero<Number>().get(), BoundType::INFTY);
		} else if (i.lower_bound_type() == BoundType::INFTY) {
			if (i.contains(carl::constant_zero<Number>().get())) {
				return Interval<Number>(carl::constant_zero<Number>().get(), BoundType::WEAK, carl::constant_zero<Number>().get(), BoundType::INFTY);
			} else {
				return Interval<Number>(boost::numeric::pow(i.content(), int(exp)), i.upper_bound_type(), BoundType::INFTY);
			}
		} else if (i.upper_bound_type() == BoundType::INFTY) {
			if (i.contains(carl::constant_zero<Number>().get())) {
				return Interval<Number>(carl::constant_zero<Number>().get(), BoundType::WEAK, carl::constant_zero<Number>().get(), BoundType::INFTY);
			} else {
				return Interval<Number>(boost::numeric::pow(i.content(), int(exp)), i.lower_bound_type(), BoundType::INFTY);
			}
		} else {
			BoundType rType = i.upper_bound_type();
			BoundType lType = i.lower_bound_type();
			if (carl::abs(i.lower()) > carl::abs(i.upper())) {
				std::swap(lType, rType);
			}
			if (i.contains(carl::constant_zero<Number>().get())) {
				return Interval<Number>(boost::numeric::pow(i.content(), int(exp)), BoundType::WEAK, rType);
			} else {
				return Interval<Number>(boost::numeric::pow(i.content(), int(exp)), lType, rType);
			}
		}
	} else {
		return Interval<Number>(boost::numeric::pow(i.content(), int(exp)), i.lower_bound_type(), i.upper_bound_type());
	}
}

template<typename Number, typename Integer>
void pow_assign(Interval<Number>& i, Integer exp) {
	i = pow(i, exp);
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
Interval<Number> sqrt(const Interval<Number>& i) {
	assert(i.isConsistent());
	Interval<Number> res;
	if (i.upper_bound_type() != BoundType::INFTY && i.upper() < carl::constant_zero<Number>().get()) {
		res = Interval<Number>::empty_interval();
	}
	else if (i.lower_bound_type() == BoundType::INFTY || i.lower() < carl::constant_zero<Number>().get()) {
		if (i.upper_bound_type() == BoundType::INFTY) {
			res = Interval<Number>(carl::constant_zero<Number>().get(), BoundType::WEAK, i.upper_bound_type());
		} else {
			res = Interval<Number>(boost::numeric::sqrt(typename Interval<Number>::BoostInterval(carl::constant_zero<Number>().get(),i.upper())), BoundType::WEAK, i.upper_bound_type());
		}
	} else {
		res = Interval<Number>(boost::numeric::sqrt(i.content()), i.lower_bound_type(), i.upper_bound_type());
	}
	CARL_LOG_DEBUG("carl.interval", "sqrt(" << i << ") = " << res);
	return res;
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void sqrt_assign(Interval<Number>& i) {
	i = sqrt(i, exp);
}

}