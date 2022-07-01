#pragma once

#include "Interval.h"

namespace carl {

/**
 * Returns the center point of the interval.
 * @return Center.
 */
template<typename Number>
Number center(const Interval<Number>& i) {
	assert(i.is_consistent() && !i.is_empty());
	if (i.is_infinite()) return carl::constant_zero<Number>().get();
	if (i.lower_bound_type() == BoundType::INFTY) {
		return carl::floor(i.upper()) - carl::constant_one<Number>().get();
	}
	if (i.upper_bound_type() == BoundType::INFTY) {
		return carl::ceil(i.lower()) + carl::constant_one<Number>().get();
	}
	return boost::numeric::median(i.content());
}

/**
 * Searches for some point in this interval, preferably near the midpoint and with a small representation.
 * Checks the integers next to the midpoint, uses the midpoint if both are outside.
 * @return Some point within this interval.
 */
template<typename Number>
Number sample(const Interval<Number>& i, bool includingBounds = true) {
	assert(i.is_consistent() && !i.is_empty());
	assert(includingBounds || !i.is_point_interval());
	Number mid = center(i);
	if (Number midf = carl::floor(mid); i.contains(midf) && (includingBounds || i.lower_bound_type() == BoundType::INFTY || i.lower() < midf)) {
		return midf;
	}
	if (Number midc = carl::ceil(mid); i.contains(midc) && (includingBounds || i.upper_bound_type() == BoundType::INFTY || i.upper() > midc)) {
		return midc;
	}
	return mid;
}

/**
 * Searches for some point in this interval, preferably near the midpoint and with a small representation.
 * Uses a binary search based on the Stern-Brocot tree starting from the integer below the midpoint.
 * @return Some point within this interval.
 */
template<typename Number>
Number sample_stern_brocot(const Interval<Number>& i, bool includingBounds = true) {
	assert(i.is_consistent() && !i.is_empty());

	using Int = typename carl::IntegralType<Number>::type;
	Int leftnum = Int(carl::floor(center(i)));
	Int leftden = carl::constant_one<Int>::get();
	Int rightnum = carl::constant_one<Int>::get();
	Int rightden = carl::constant_zero<Int>::get();
	Number cur = Number(leftnum) / Number(leftden);
	if (i.contains(cur)) {
		return cur;
	}
	while (true) {
		Int curnum = leftnum + rightnum;
		Int curden = leftden + rightden;
		cur = Number(curnum) / Number(curden);
		if ((cur < i.lower()) || (!includingBounds && cur == i.lower())) {
			leftnum = curnum;
			leftden = curden;
		} else if ((cur > i.upper()) || (!includingBounds && cur == i.upper())) {
			rightnum = curnum;
			rightden = curden;
		} else {
			return cur;
		}
	}
}

/**
 * Searches for some point in this interval, preferably near the left endpoint and with a small representation.
 * Checks the integer next to the left endpoint, uses the midpoint if it is outside.
 * @return Some point within this interval.
 */
template<typename Number>
Number sample_left(const Interval<Number>& i) {
	assert(i.is_consistent() && !i.is_empty());
	if (i.lower_bound_type() == BoundType::INFTY) {
		return carl::floor(i.upper()) - carl::constant_one<Number>().get();
	}
	Number res = carl::floor(i.lower()) + carl::constant_one<Number>().get();
	if (i.contains(res)) return res;
	return center(i);
}
/**
 * Searches for some point in this interval, preferably near the right endpoint and with a small representation.
 * Checks the integer next to the right endpoint, uses the midpoint if it is outside.
 * @return Some point within this interval.
 */
template<typename Number>
Number sample_right(const Interval<Number>& i) {
	assert(i.is_consistent() && !i.is_empty());
	if (i.upper_bound_type() == BoundType::INFTY) {
		return carl::ceil(i.lower()) + carl::constant_one<Number>().get();
	}
	Number res = carl::ceil(i.upper()) - carl::constant_one<Number>().get();
	if (i.contains(res)) return res;
	return center(i);
}
/**
* Searches for some point in this interval, preferably near zero and with a small representation.
* Checks the integer next to the left endpoint if the interval is semi-positive.
* Checks the integer next to the right endpoint if the interval is semi-negative.
* Uses zero otherwise.
* @return Some point within this interval.
*/
template<typename Number>
Number sample_zero(const Interval<Number>& i) {
	assert(i.is_consistent() && !i.is_empty());
	if (i.is_semi_positive()) return sample_left(i);
	if (i.is_semi_negative()) return sample_right(i);
	return carl::constant_zero<Number>::get();
}
/**
* Searches for some point in this interval, preferably far aways from zero and with a small representation.
* Checks the integer next to the right endpoint if the interval is semi-positive.
* Checks the integer next to the left endpoint if the interval is semi-negative.
* Uses zero otherwise.
* @return Some point within this interval.
*/
template<typename Number>
Number sample_infty(const Interval<Number>& i) {
	assert(i.is_consistent() && !i.is_empty());
	if (i.is_semi_positive()) return sample_right(i);
	if (i.is_semi_negative()) return sample_left(i);
	return carl::constant_zero<Number>::get();
}


}