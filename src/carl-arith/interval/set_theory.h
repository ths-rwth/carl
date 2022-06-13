#pragma once

#include "Interval.h"

namespace carl {

/**
 * Calculates the complement in a set-theoretic manner (can result
 * in two distinct intervals).
 * @param interval Interval.
 * @param resA Result a.
 * @param resB Result b.
 * @return True, if the result is twofold.
 */
template<typename Number>
bool set_complement(const Interval<Number>& interval, Interval<Number>& resA, Interval<Number>& resB) {
	assert(interval.is_consistent());

	if (interval.lower_bound_type() == BoundType::INFTY) {
		if (interval.upper_bound_type() == BoundType::INFTY) {
			resA = Interval<Number>::empty_interval();
			resB = Interval<Number>::empty_interval();
			return false;
		} else {
			resA = Interval<Number>(interval.upper(), get_other_bound_type(interval.upper_bound_type()), 0, BoundType::INFTY);
			resB = Interval<Number>::empty_interval();
			return false;
		}
	} else {
		if (interval.upper_bound_type() == BoundType::INFTY) {
			resA = Interval<Number>(0, BoundType::INFTY, interval.lower(), get_other_bound_type(interval.lower_bound_type()));
			resB = Interval<Number>::empty_interval();
			return false;
		} else {
			resA = Interval<Number>(0, BoundType::INFTY, interval.lower(), get_other_bound_type(interval.lower_bound_type()));
			resB = Interval<Number>(interval.upper(), get_other_bound_type(interval.upper_bound_type()), 0, BoundType::INFTY);
			return true;
		}
	}
}

/**
 * Calculates the difference of two intervals in a set-theoretic manner:
 * lhs \ rhs (can result in two distinct intervals).
 * @param lhs First interval.
 * @param rhs Second interval.
 * @param resA Result a.
 * @param resB Result b.
 * @return True, if the result is twofold.
 */
template<typename Number>
bool set_difference(const Interval<Number>& lhs, const Interval<Number>& rhs, Interval<Number>& resA, Interval<Number>& resB) {
	assert(lhs.is_consistent() && rhs.is_consistent());

	if (rhs.is_empty()) {
		resA = lhs;
		resB = Interval<Number>::empty_interval();
		return false;
	}
	if (lhs.upper_bound() < rhs.lower_bound()) {
		resA = lhs;
		resB = Interval<Number>::empty_interval();
		return false;
	}
	if (rhs.upper_bound() < lhs.lower_bound()) {
		resA = lhs;
		resB = Interval<Number>::empty_interval();
		return false;
	}
	if (lhs.lower_bound() < rhs.lower_bound()) {
		if (lhs.upper_bound() < rhs.upper_bound()) {
			resA = Interval<Number>(lhs.lower_bound(), rhs.lower_bound());
			return false;
		} else {
			resA = Interval<Number>(lhs.lower_bound(), rhs.lower_bound());
			resB = Interval<Number>(rhs.upper_bound(), lhs.upper_bound());
			return !resB.is_empty();
		}
	} else {
		if (lhs.upper_bound() < rhs.upper_bound()) {
			resA = Interval<Number>::empty_interval();
			resB = Interval<Number>::empty_interval();
			return false;
		} else {
			resA = Interval<Number>(rhs.upper_bound(), lhs.upper_bound());
			resB = Interval<Number>::empty_interval();
			return false;
		}
	}
}

/**
 * Intersects two intervals in a set-theoretic manner.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Result.
 */
template<typename Number>
Interval<Number> set_intersection(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	assert(lhs.is_consistent() && rhs.is_consistent());

	if (lhs.lower_bound() < rhs.lower_bound()) {
		if (lhs.upper_bound() < rhs.upper_bound()) {
			return Interval<Number>(rhs.lower_bound(), lhs.upper_bound());
		} else {
			return rhs;
		}
	} else {
		if (lhs.upper_bound() < rhs.upper_bound()) {
			return lhs;
		} else {
			return Interval<Number>(lhs.lower_bound(), rhs.upper_bound());
		}
	}
}

template<typename Number>
bool set_have_intersection(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	if (lhs.lower_bound() <= rhs.lower_bound() && rhs.lower_bound() <= lhs.upper_bound()) return true;
	if (rhs.lower_bound() <= lhs.lower_bound() && lhs.lower_bound() <= rhs.upper_bound()) return true;
	return false;
}

/**
 * Checks whether lhs is a proper subset of rhs.
 */
template<typename Number>
bool set_is_proper_subset(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	assert(lhs.is_consistent() && rhs.is_consistent());

	if (lhs.is_empty()) return !rhs.is_empty();
	if (lhs.lower_bound() < rhs.lower_bound()) return false;
	if (rhs.upper_bound() < lhs.upper_bound()) return false;
	if (rhs.lower_bound() < lhs.lower_bound()) return true;
	if (lhs.upper_bound() < rhs.upper_bound()) return true;
	return false;
}

/**
 * Checks whether lhs is a subset of rhs.
 */
template<typename Number>
bool set_is_subset(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	assert(lhs.is_consistent() && rhs.is_consistent());

	if (lhs.is_empty()) return true;
	if (lhs.lower_bound() < rhs.lower_bound()) return false;
	if (rhs.upper_bound() < lhs.upper_bound()) return false;
	return true;
}

/**
 * Calculates the symmetric difference of two intervals in a
 * set-theoretic manner (can result in two distinct intervals).
 * @param lhs First interval.
 * @param rhs Second interval.
 * @param resA Result a.
 * @param resB Result b.
 * @return True, if the result is twofold.
 */
template<typename Number>
bool set_symmetric_difference(const Interval<Number>& lhs, const Interval<Number>& rhs, Interval<Number>& resA, Interval<Number>& resB) {
	assert(lhs.is_consistent() && rhs.is_consistent());

	auto intersection = set_intersection(lhs, rhs);
	if (intersection.is_empty()) {
		resA = lhs;
		resB = rhs;
		return true;
	} else {
		Interval<Number> tmp;
		bool res = set_union(lhs, rhs, tmp, tmp);
		assert(!res);
		return set_difference(tmp, intersection, resA, resB);
	}
}

/**
 * Computes the union of two intervals (can result in two distinct intervals).
 * @param lhs First interval.
 * @param rhs Second interval.
 * @param resA Result a.
 * @param resB Result b.
 * @return True, if the result is twofold.
 */
template<typename Number>
bool set_union(const Interval<Number>& lhs, const Interval<Number>& rhs, Interval<Number>& resA, Interval<Number>& resB) {
	assert(lhs.is_consistent() && rhs.is_consistent());

	if (carl::bounds_connect(lhs.upper_bound(), rhs.lower_bound())) {
		resA = Interval<Number>(lhs.lower_bound(), rhs.upper_bound());
		resB = Interval<Number>::empty_interval();
		return false;
	}
	if (carl::bounds_connect(rhs.upper_bound(), lhs.lower_bound())) {
		resA = Interval<Number>(rhs.lower_bound(), lhs.upper_bound());
		resB = Interval<Number>::empty_interval();
		return false;
	}
	if (lhs.upper_bound() < rhs.lower_bound()) {
		resA = lhs;
		resB = rhs;
		return true;
	}
	if (rhs.upper_bound() < lhs.lower_bound()) {
		resA = rhs;
		resB = lhs;
		return true;
	}
	if (lhs.lower_bound() < rhs.lower_bound()) {
		if (lhs.upper_bound() < rhs.upper_bound()) {
			resA = Interval<Number>(lhs.lower_bound(), rhs.upper_bound());
		} else {
			resA = lhs;
		}
	} else {
		if (lhs.upper_bound() < rhs.upper_bound()) {
			resA = rhs;
		} else {
			resA = Interval<Number>(rhs.lower_bound(), lhs.upper_bound());
		}
	}
	resB = Interval<Number>::empty_interval();
	return false;
}

}