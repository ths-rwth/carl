#pragma once

#include "Interval.h"

namespace carl {

template<typename Number>
Interval<Number> set_intersection(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	assert(lhs.isConsistent() && rhs.isConsistent());

	if (lhs.lowerBound() < rhs.lowerBound()) {
		if (lhs.upperBound() < rhs.upperBound()) {
			return Interval<Number>(rhs.lowerBound(), lhs.upperBound());
		} else {
			return rhs;
		}
	} else {
		if (lhs.upperBound() < rhs.upperBound()) {
			return lhs;
		} else {
			return Interval<Number>(lhs.lowerBound(), rhs.upperBound());
		}
	}
}

template<typename Number>
bool set_intersect(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	if (rhs.lowerBound() < lhs.lowerBound() && lhs.upperBound() < rhs.upperBound()) return true;
	if (lhs.lowerBound() < rhs.lowerBound() && rhs.upperBound() < lhs.upperBound()) return true;
	return false;
}

/**
 * Checks whether lhs is a subset of rhs.
 */
template<typename Number>
bool set_subset(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	assert(lhs.isConsistent() && rhs.isConsistent());

	if (lhs.isEmpty()) return true;
	if (lhs.lowerBound() < rhs.lowerBound()) return false;
	if (rhs.upperBound() < lhs.upperBound()) return false;
	return true;
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
	assert(lhs.isConsistent() && rhs.isConsistent());
	
	if (carl::bounds_connect(lhs.upperBound(), rhs.lowerBound())) {
		resA = Interval<Number>(lhs.lowerBound(), rhs.upperBound());
		resB = Interval<Number>::emptyInterval();
		return false;
	}
	if (carl::bounds_connect(rhs.upperBound(), lhs.lowerBound())) {
		resA = Interval<Number>(rhs.lowerBound(), lhs.upperBound());
		resB = Interval<Number>::emptyInterval();
		return false;
	}
	if (lhs.upperBound() < rhs.lowerBound()) {
		resA = lhs;
		resB = rhs;
		return true;
	}
	if (rhs.upperBound() < lhs.lowerBound()) {
		resA = rhs;
		resB = lhs;
		return true;
	}
	if (lhs.lowerBound() < rhs.lowerBound()) {
		if (lhs.upperBound() < rhs.upperBound()) {
			resA = Interval<Number>(lhs.lowerBound(), rhs.upperBound());
		} else {
			resA = lhs;
		}
	} else {
		if (lhs.upperBound() < rhs.upperBound()) {
			resA = rhs;
		} else {
			resA = Interval<Number>(rhs.lowerBound(), lhs.upperBound());
		}
	}
	resB = Interval<Number>::emptyInterval();
	return false;
}

/**
 * Calculates the difference of two intervals in a set-theoretic manner:
 * lhs \ rhs (can result in two distinct intervals).
 * @param rhs Righthand side.
 * @param resultA Result a.
 * @param resultB Result b.
 * @return True, if the result is twofold.
 */
template<typename Number>
bool set_difference(const Interval<Number>& lhs, const Interval<Number>& rhs, Interval<Number>& resA, Interval<Number>& resB) {
	assert(lhs.isConsistent() && rhs.isConsistent());
	
	if (rhs.isEmpty()) {
		resA = lhs;
		resB = Interval<Number>::emptyInterval();
		return false;
	}
	if (lhs.upperBound() < rhs.lowerBound()) {
		resA = lhs;
		resB = Interval<Number>::emptyInterval();
		return false;
	}
	if (rhs.upperBound() < lhs.lowerBound()) {
		resA = lhs;
		resB = Interval<Number>::emptyInterval();
		return false;
	}
	if (lhs.lowerBound() < rhs.lowerBound()) {
		if (lhs.upperBound() < rhs.upperBound()) {
			resA = Interval<Number>(lhs.lowerBound(), rhs.lowerBound());
			return false;
		} else {
			resA = Interval<Number>(lhs.lowerBound(), rhs.lowerBound());
			resB = Interval<Number>(rhs.upperBound(), lhs.upperBound());
			return !resB.isEmpty();
		}
	} else {
		if (lhs.upperBound() < rhs.upperBound()) {
			resA = Interval<Number>::emptyInterval();
			resB = Interval<Number>::emptyInterval();
			return false;
		} else {
			resA = Interval<Number>(rhs.upperBound(), lhs.upperBound());
			resB = Interval<Number>::emptyInterval();
			return false;
		}
	}
}

}