/**
 * @file:   RealAlgebraicNumberOperations.h
 * @author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 *
 * This file should never be included directly but only via RealAlgebraicNumber.h
 */

#include "RealAlgebraicNumberIR.h"
#include "pointerOperations.h"

#pragma once

namespace std {

/**
 * Specialization of `std::equal_to` for RealAlgebraicNumberPtr.
 */
template<typename Number>
struct equal_to<carl::RealAlgebraicNumberPtr<Number>> {
	/**
	 * Checks lhs and rhs for equality.
	 * @param lhs First number.
	 * @param rhs Second number.
	 * @return `lhs == rhs`.
	 */
	bool operator()(const carl::RealAlgebraicNumberPtr<Number>& lhs, const carl::RealAlgebraicNumberPtr<Number>& rhs) const {
		if (lhs == rhs) return true;
		if (lhs->isNumeric()) {
			if (rhs->isNumeric()) {
				return lhs->value() == rhs->value();
			} else {
				if (!std::static_pointer_cast<carl::RealAlgebraicNumberIR<Number>>(rhs)->refineAvoiding(lhs->value())) {
					return false;
				}
			}
		} else {
			auto lhsIR = std::static_pointer_cast<carl::RealAlgebraicNumberIR<Number>>(lhs);
			if (rhs->isNumeric()) {
				if (!lhsIR->refineAvoiding(rhs->value())) {
					return false;
				}
			} else {
				auto rhsIR = std::static_pointer_cast<carl::RealAlgebraicNumberIR<Number>>(rhs);
				return lhsIR->equal(rhsIR);
			}
		}
		// nrA must be the exact numeric representation of irB OR nrB must be the exact numeric representation of irA
		return true;
	}
};

/**
 * Specialization of `std::equal_to` for RealAlgebraicNumberIRPtr.
 */
template<typename Number>
struct equal_to<carl::RealAlgebraicNumberIRPtr<Number>> {
	/// Equality operator for RealAlgebraicNumberPtr.
	equal_to<carl::RealAlgebraicNumberPtr<Number>> eq;
	/**
	 * Calls `eq(lhs, rhs)`.
	 * @param lhs First number.
	 * @param rhs Second number.
	 * @return `lhs == rhs`.
	 */
	bool operator()(const carl::RealAlgebraicNumberIRPtr<Number>& lhs, const carl::RealAlgebraicNumberIRPtr<Number>& rhs) const {
		return eq(lhs, rhs);
	}
};

/**
 * Specialization of `std::equal_to` for RealAlgebraicNumberNRPtr.
 */
template<typename Number>
struct equal_to<carl::RealAlgebraicNumberNRPtr<Number>> {
	/// Equality operator for RealAlgebraicNumberPtr.
	equal_to<carl::RealAlgebraicNumberPtr<Number>> eq;
	/**
	 * Calls `eq(lhs, rhs)`.
	 * @param lhs First number.
	 * @param rhs Second number.
	 * @return `lhs == rhs`.
	 */
	bool operator()(const carl::RealAlgebraicNumberNRPtr<Number>& lhs, const carl::RealAlgebraicNumberNRPtr<Number>& rhs) const {
		return eq(lhs, rhs);
	}
};

/**
 * Specialization of `std::not_equal_to` for RealAlgebraicNumberPtr.
 */
template<typename Number>
struct not_equal_to<carl::RealAlgebraicNumberPtr<Number>> {
	/// Equality operator for RealAlgebraicNumberPtr.
	std::equal_to<carl::RealAlgebraicNumberPtr<Number>> eq;
	/**
	 * Calls `!eq(lhs, rhs)`.
	 * @param lhs First number.
	 * @param rhs Second number.
	 * @return `lhs != rhs`.
	 */
	bool operator()(carl::RealAlgebraicNumberPtr<Number> lhs, carl::RealAlgebraicNumberPtr<Number> rhs) const {
		return !eq(lhs, rhs);
	}
};

/**
 * Specialization of `std::less` for RealAlgebraicNumberPtr.
 */
template<typename Number>
struct less<carl::RealAlgebraicNumberPtr<Number>> {
	/// Equality operator for RealAlgebraicNumberPtr.
	std::equal_to<carl::RealAlgebraicNumberPtr<Number>> eq;
	/**
	 * Checks if one number is smaller than another.
	 * @param lhs First number.
	 * @param rhs Second number.
	 * @return `lhs < rhs`.
     */
	bool operator()(carl::RealAlgebraicNumberPtr<Number> lhs, carl::RealAlgebraicNumberPtr<Number> rhs) const {
		assert(lhs != nullptr);
		assert(rhs != nullptr);
		if (lhs == rhs) return false;
		if (eq(lhs,rhs)) return false;
		if (lhs->isNumeric()) {
			if (rhs->isNumeric()) {
				return lhs->value() < rhs->value();
			} else {
				auto rhsIR = std::static_pointer_cast<carl::RealAlgebraicNumberIR<Number>>(rhs);
				rhsIR->refineAvoiding(lhs->value());
				if (rhs->isNumeric()) {
					return lhs->value() < rhs->value();
				} else {
					return lhs->value() <= std::static_pointer_cast<carl::RealAlgebraicNumberIR<Number>>(rhs)->lower();
				}
			}
		} else {
			auto lhsIR = std::static_pointer_cast<carl::RealAlgebraicNumberIR<Number>>(lhs);
			if (rhs->isNumeric()) {
				lhsIR->refineAvoiding(rhs->value());
				if (lhs->isNumeric()) {
					return lhs->value() < rhs->value();
				} else {
					return lhsIR->upper() <= rhs->value();
				}
			} else {
				auto rhsIR = std::static_pointer_cast<carl::RealAlgebraicNumberIR<Number>>(rhs);
				if (lhsIR->equal(rhsIR)) return false;
				return std::const_pointer_cast<carl::RealAlgebraicNumberIR<Number>>(lhsIR)->lessWhileUnequal(rhsIR);
			}
		}
	}
};

/**
 * Specialization of `std::greater` for RealAlgebraicNumberPtr.
 */
template<typename Number>
struct greater<carl::RealAlgebraicNumberPtr<Number>> {
	/// Less operator for RealAlgebraicNumberPtr.
	std::less<carl::RealAlgebraicNumberPtr<Number>> less;
	/**
	 * Calls `less(rhs, lhs)`.
	 * @param lhs First number.
	 * @param rhs Second number.
	 * @return `lhs > rhs`.
	 */
	bool operator()(carl::RealAlgebraicNumberPtr<Number> lhs, carl::RealAlgebraicNumberPtr<Number> rhs) const {
		return less(rhs, lhs);
	}
};

}
