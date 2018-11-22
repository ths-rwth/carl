#pragma once

#include "Interval.h"

#include <cassert>


namespace carl {

template<typename Number, typename Integer>
Interval<Number> pow(const Interval<Number>& i, Integer exp) {
	assert(i.isConsistent());
	if (exp % 2 == 0) {
		if (i.isInfinite()) {
			return Interval<Number>(carl::constant_zero<Number>().get(), BoundType::WEAK, carl::constant_zero<Number>().get(), BoundType::INFTY);
		} else if (i.lowerBoundType() == BoundType::INFTY) {
			if (i.contains(carl::constant_zero<Number>().get())) {
				return Interval<Number>(carl::constant_zero<Number>().get(), BoundType::WEAK, carl::constant_zero<Number>().get(), BoundType::INFTY);
			} else {
				return Interval<Number>(boost::numeric::pow(i.rContent(), int(exp)), i.upperBoundType(), BoundType::INFTY);
			}
		} else if (i.upperBoundType() == BoundType::INFTY) {
			if (i.contains(carl::constant_zero<Number>().get())) {
				return Interval<Number>(carl::constant_zero<Number>().get(), BoundType::WEAK, carl::constant_zero<Number>().get(), BoundType::INFTY);
			} else {
				return Interval<Number>(boost::numeric::pow(i.rContent(), int(exp)), i.lowerBoundType(), BoundType::INFTY);
			}
		} else {
			BoundType rType = i.upperBoundType();
			BoundType lType = i.lowerBoundType();
			if (carl::abs(i.lower()) > carl::abs(i.upper())) {
				std::swap(lType, rType);
			}
			if (i.contains(carl::constant_zero<Number>().get())) {
				return Interval<Number>(boost::numeric::pow(i.rContent(), int(exp)), BoundType::WEAK, rType);
			} else {
				return Interval<Number>(boost::numeric::pow(i.rContent(), int(exp)), lType, rType);
			}
		}
	} else {
		return Interval<Number>(boost::numeric::pow(i.rContent(), int(exp)), i.lowerBoundType(), i.upperBoundType());
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
	if (i.upperBoundType() != BoundType::INFTY && i.upper() < carl::constant_zero<Number>().get()) {
		res = Interval<Number>::emptyInterval();
	}
	else if (i.lowerBoundType() == BoundType::INFTY || i.lower() < carl::constant_zero<Number>().get()) {
		if (i.upperBoundType() == BoundType::INFTY) {
			res = Interval<Number>(carl::constant_zero<Number>().get(), BoundType::WEAK, i.upperBoundType());
		} else {
			res = Interval<Number>(boost::numeric::sqrt(typename Interval<Number>::BoostInterval(carl::constant_zero<Number>().get(),i.upper())), BoundType::WEAK, i.upperBoundType());
		}
	} else {
		res = Interval<Number>(boost::numeric::sqrt(i.rContent()), i.lowerBoundType(), i.upperBoundType());
	}
	CARL_LOG_DEBUG("carl.interval", "sqrt(" << i << ") = " << res);
	return res;
}

template<typename Number, EnableIf<std::is_floating_point<Number>> = dummy>
void sqrt_assign(Interval<Number>& i) {
	i = sqrt(i, exp);
}

}