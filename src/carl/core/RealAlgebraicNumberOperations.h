/* 
 * File:   RealAlgebraicNumberOperations.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * This file should never be included directly but only via RealAlgebraicNumber.h
 */

#include "RealAlgebraicNumberIR.h"


#pragma once

namespace carl {

template<typename Number>
struct Equal {
	bool operator()(RealAlgebraicNumberPtr<Number> lhs, RealAlgebraicNumberPtr<Number> rhs) {
		if (lhs == rhs) return true;
		if (lhs->isNumeric()) {
			if (rhs->isNumeric()) {
				return lhs->value() == rhs->value();
			} else {
				if (!std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(rhs)->refineAvoiding(lhs->value())) {
					return false;
				}
			}
		} else {
			auto lhsIR = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(lhs);
			if (rhs->isNumeric()) {
				if (!lhsIR->refineAvoiding(rhs->value())) {
					return false;
				}
			} else {
				auto rhsIR = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(rhs);
				return lhsIR->equal(rhsIR);
			}
		}
		// nrA must be the exact numeric representation of irB OR nrB must be the exact numeric representation of irA
		return true;
	}
};
template<typename Number>
struct Inequal {
private:
	Equal<Number> e;
public:
	bool operator()(RealAlgebraicNumberPtr<Number>& lhs, RealAlgebraicNumberPtr<Number>& rhs) {
		return !e(lhs, rhs);
	}
};


template<typename Number>
struct Less {
	bool operator()(RealAlgebraicNumberPtr<Number> lhs, RealAlgebraicNumberPtr<Number> rhs) {
		if (lhs == rhs) return false;
		if (lhs->isNumeric()) {
			if (rhs->isNumeric()) {
				return lhs->value() < rhs->value();
			} else {
				return lhs->value() < std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(rhs)->left();
			}
		} else {
			auto lhsIR = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(lhs);
			if (rhs->isNumeric()) {
				return lhsIR->right() < rhs->value();
			} else {
				auto rhsIR = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(rhs);
				if (lhsIR->equal(rhsIR)) return false;
				return std::const_pointer_cast<RealAlgebraicNumberIR<Number>>(lhsIR)->lessWhileUnequal(rhsIR);
			}
		}
	}
};

template<typename Number>
struct Greater {
private:
	Less<Number> l;
public:
	bool operator()(RealAlgebraicNumberPtr<Number> lhs, RealAlgebraicNumberPtr<Number> rhs) {
		return l(rhs, lhs);
	}
};

}