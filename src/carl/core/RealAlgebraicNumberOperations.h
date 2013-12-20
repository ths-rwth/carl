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
	bool operator()(RealAlgebraicNumber<Number>* lhs, RealAlgebraicNumber<Number>* rhs) {
		if (lhs == rhs) return true;
		if (lhs->isNumeric()) {
			if (rhs->isNumeric()) {
				return lhs->value() == rhs->value();
			} else {
				if (!static_cast<RealAlgebraicNumberIR<Number>*>(rhs)->refineAvoiding(lhs->value())) {
					return false;
				}
			}
		} else {
			auto lhsIR = static_cast<RealAlgebraicNumberIR<Number>*>(lhs);
			if (rhs->isNumeric()) {
				if (!lhsIR->refineAvoiding(rhs->value())) {
					return false;
				}
			} else {
				return lhsIR->equal(static_cast<RealAlgebraicNumberIR<Number>*>(rhs));
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
	bool operator()(RealAlgebraicNumber<Number>* lhs, RealAlgebraicNumber<Number>* rhs) {
		return !e(lhs, rhs);
	}
};


template<typename Number>
struct Less {
	bool operator()(const RealAlgebraicNumber<Number>* lhs, const RealAlgebraicNumber<Number>* rhs) {
		if (lhs == rhs) return false;
		if (lhs->isNumeric()) {
			if (rhs->isNumeric()) {
				return lhs->value() < rhs->value();
			} else {
				return lhs->value() < static_cast<const RealAlgebraicNumberIR<Number>*>(rhs)->left();
			}
		} else {
			auto lhsIR = static_cast<const RealAlgebraicNumberIR<Number>*>(lhs);
			if (rhs->isNumeric()) {
				return lhsIR->right() < rhs->value();
			} else {
				auto rhsIR = const_cast<RealAlgebraicNumberIR<Number>*>(static_cast<const RealAlgebraicNumberIR<Number>*>(rhs));
				return const_cast<RealAlgebraicNumberIR<Number>*>(lhsIR)->lessWhileUnequal(rhsIR);
			}
		}
	}
};

template<typename Number>
struct Greater {
private:
	Less<Number> l;
public:
	bool operator()(RealAlgebraicNumber<Number>* const lhs, RealAlgebraicNumber<Number>* const rhs) {
		return l(rhs, lhs);
	}
};

}