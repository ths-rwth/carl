#pragma once

#include "Interval.h"

namespace carl {

/**
 * Operator for the comparison of two intervals.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return True if both intervals are equal.
 */
template<typename Number>
inline bool operator==(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	return std::forward_as_tuple(lhs.lowerBoundType(), lhs.upperBoundType(), lhs.lower(), lhs.upper())
		== std::forward_as_tuple(rhs.lowerBoundType(), rhs.upperBoundType(), rhs.lower(), rhs.upper());
}
template<typename Number>
inline bool operator==(const Interval<Number>& lhs, const Number& rhs) {
	return lhs.isPointInterval() && lhs.lower() == rhs;
}
template<typename Number>
inline bool operator==(const Number& lhs, const Interval<Number>& rhs) {
	return rhs == lhs;
}

/**
 * Operator for the comparison of two intervals.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return True if both intervals are unequal.
 */
template<typename Number>
inline bool operator!=(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	return !(lhs == rhs);
}
template<typename Number>
inline bool operator!=(const Interval<Number>& lhs, const Number& rhs) {
	return !(lhs == rhs);
}
template<typename Number>
inline bool operator!=(const Number& lhs, const Interval<Number>& rhs) {
	return !(lhs == rhs);
}

/**
 * Operator for the comparison of two intervals.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return True if the lefthand side is smaller than the righthand side.
 */
template<typename Number>
inline bool operator<(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	if (lhs.upper() < rhs.lower()) return true;
	if (lhs.upper() == rhs.lower()) {
		switch (lhs.upperBoundType()) {
			case BoundType::STRICT:
				return rhs.lowerBoundType() != BoundType::INFTY;
			case BoundType::WEAK:
				return rhs.lowerBoundType() == BoundType::STRICT;
			default:
				return false;
		}
	}
	return false;
}
template<typename Number>
inline bool operator<(const Interval<Number>& lhs, const Number& rhs) {
	switch (lhs.upperBoundType()) {
		case BoundType::STRICT:
			return lhs.upper() <= rhs;
		case BoundType::WEAK:
			return lhs.upper() < rhs;
		case BoundType::INFTY:
			return false;
	}
}
template<typename Number>
inline bool operator<(const Number& lhs, const Interval<Number>& rhs) {
	switch (rhs.lowerBoundType()) {
		case BoundType::STRICT:
			return lhs <= rhs.lower();
		case BoundType::WEAK:
			return lhs < rhs.lower();
		case BoundType::INFTY:
			return false;
	}
}

/**
 * Operator for the comparison of two intervals.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return True if the lefthand side is larger than the righthand side.
 */
template<typename Number>
inline bool operator>(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	return rhs < lhs;
}
template<typename Number>
inline bool operator>(const Interval<Number>& lhs, const Number& rhs) {
	return rhs < lhs;
}
template<typename Number>
inline bool operator>(const Number& lhs, const Interval<Number>& rhs) {
	return rhs < lhs;
}


/**
 * Operator for the comparison of two intervals.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return True if the righthand side has maximal one intersection with the lefthand side
 * at the upper bound of lhs.
 */
template<typename Number>
inline bool operator<=(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	if (lhs < rhs) return true;
	if (lhs.upper() == rhs.lower()) {
		switch (lhs.upperBoundType()) {
			case BoundType::STRICT:
			case BoundType::WEAK:
				return rhs.lowerBoundType() != BoundType::INFTY;
			default:
				return false;
		}
	}
	return false;
}
template<typename Number>
inline bool operator<=(const Interval<Number>& lhs, const Number& rhs) {
	if (lhs.upperBoundType() == BoundType::INFTY) return false;
	return lhs.upper() <= rhs;
}
template<typename Number>
inline bool operator<=(const Number& lhs, const Interval<Number>& rhs) {
	if (rhs.lowerBoundType() == BoundType::INFTY) return false;
	return rhs.lower() >= lhs;
}

/**
 * Operator for the comparison of two intervals.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return True if the lefthand side has maximal one intersection with the righthand side
 * at the lower bound of lhs.
 */
template<typename Number>
inline bool operator>=(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	return rhs <= lhs;
}
template<typename Number>
inline bool operator>=(const Interval<Number>& lhs, const Number& rhs) {
	return rhs <= lhs;
}
template<typename Number>
inline bool operator>=(const Number& lhs, const Interval<Number>& rhs) {
	return rhs <= lhs;
}


/**
 * Operator for the addition of two intervals.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number> operator+(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	return lhs.add(rhs);
}

/**
 * Operator for the addition of an interval and a number.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number> operator+(const Interval<Number>& lhs, const Number& rhs) {
	return Interval<Number>(lhs.rContent() + rhs, lhs.lowerBoundType(), lhs.upperBoundType());
}

/**
 * Operator for the addition of an interval and a number.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number> operator+(const Number& lhs, const Interval<Number>& rhs) {
	return rhs + lhs;
}


/**
 * Operator for the addition of an interval and a number with assignment.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number>& operator+=(Interval<Number>& lhs, const Interval<Number>& rhs) {
	lhs.add_assign(rhs);
	return lhs;
}

/**
 * Operator for the addition of an interval and a number with assignment.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number>& operator+=(Interval<Number>& lhs, const Number& rhs) {
	lhs.rContent() += rhs;
	return lhs;
}

/**
 * Unary minus.
 * @param interval The operand.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number> operator-(const Interval<Number>& rhs) {
	return rhs.inverse();
}

/**
 * Operator for the subtraction of two intervals.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number> operator-(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	return lhs.sub(rhs);
}

/**
 * Operator for the subtraction of an interval and a number.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number> operator-(const Interval<Number>& lhs, const Number& rhs) {
	return Interval<Number>(lhs.rContent() - rhs, lhs.lowerBoundType(), lhs.upperBoundType());
}

/**
 * Operator for the subtraction of an interval and a number.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number> operator-(const Number& lhs, const Interval<Number>& rhs) {
	return (-rhs) + lhs;
}

/**
 * Operator for the subtraction of two intervals with assignment.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number>& operator-=(Interval<Number>& lhs, const Interval<Number>& rhs) {
	lhs.sub_assign(rhs);
	return lhs;
}

/**
 * Operator for the subtraction of an interval and a number with assignment.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number>& operator-=(Interval<Number>& lhs, const Number& rhs) {
	lhs.rContent() -= rhs;
	return lhs;
}

/**
 * Operator for the multiplication of two intervals.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number> operator*(const Interval<Number>& lhs, const Interval<Number>& rhs) {
	return lhs.mul(rhs);
}

/**
 * Operator for the multiplication of an interval and a number.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number> operator*(const Interval<Number>& lhs, const Number& rhs) {
	if (rhs >= 0) {
		return Interval<Number>(lhs.rContent() * rhs, lhs.lowerBoundType(), lhs.upperBoundType());
	} else {
		return Interval<Number>(lhs.rContent() * rhs, lhs.upperBoundType(), lhs.lowerBoundType());
	}
}

/**
 * Operator for the multiplication of an interval and a number.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number> operator*(const Number& lhs, const Interval<Number>& rhs) {
	return rhs * lhs;
}

/**
 * Operator for the multiplication of an interval and a number with assignment.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number>& operator*=(Interval<Number>& lhs, const Interval<Number>& rhs) {
	lhs.mul_assign(rhs);
	return lhs;
}

/**
 * Operator for the multiplication of an interval and a number with assignment.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number>& operator*=(Interval<Number>& lhs, const Number& rhs) {
	lhs.rContent() *= rhs;
	return lhs;
}

/**
 * Operator for the division of an interval and a number.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number> operator/(const Interval<Number>& lhs, const Number& rhs) {
	assert(!carl::isZero(rhs));
	if (rhs >= 0) {
		return Interval<Number>(lhs.rContent() / rhs, lhs.lowerBoundType(), lhs.upperBoundType());
	} else {
		return Interval<Number>(lhs.rContent() / rhs, lhs.upperBoundType(), lhs.lowerBoundType());
	}
}

/**
 * Operator for the division of an interval and a number with assignment.
 * @param lhs Lefthand side.
 * @param rhs Righthand side.
 * @return Resulting interval.
 */
template<typename Number>
inline Interval<Number>& operator/=(Interval<Number>& lhs, const Number& rhs) {
	return lhs = lhs / rhs;
}

}