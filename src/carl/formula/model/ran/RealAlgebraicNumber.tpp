
#include "RealAlgebraicNumber.h"

#include "../../../core/polynomialfunctions/GCD_univariate.h"

#pragma once

namespace carl {
	
	template<typename Number>
	std::pair<bool,bool> RealAlgebraicNumber<Number>::checkOrder(const RealAlgebraicNumber<Number>& n) const {
		assert(!isZ3Ran());
		if (isNumeric()) {
			if (n.isNumeric()) {
				return std::make_pair(true, value() < n.value());
			} else {
				n.refineAvoiding(value());
				if (n.isNumeric()) {
					return std::make_pair(true, value() < n.value());
				} else {
					return std::make_pair(true, value() <= n.lower());
				}
			}
		}
		if (n.isNumeric()) {
			refineAvoiding(n.value());
			if (isNumeric()) {
				return std::make_pair(true, value() < n.value());
			} else {
				return std::make_pair(true, lower() <= n.value());
			}
		}
		if (upper() <= n.lower()) {
			return std::make_pair(true, true);
		}
		if (n.upper() <= lower()) {
			return std::make_pair(true, false);
		}
		return std::make_pair(false, false);
	}
	
	template<typename Number>
	bool RealAlgebraicNumber<Number>::lessWhileUnequal(const RealAlgebraicNumber<Number>& n) const {
		assert(!isZ3Ran());
		assert(*this != n);
		
		#define CHECK_ORDER() {\
			auto res = checkOrder(n);\
			if (res.first) return res.second;\
		}
		#define INTERVAL_CONTAINED(n1, n2, twisted) {\
			auto res = n1->intervalContained(n2, twisted);\
			if (res.first) return res.second;\
		}
		
		while (true) {
			CHECK_ORDER();
			if (isInterval()) mIR->refine();
			if (n.isInterval()) n.mIR->refine();
		}
/*
			// case: is o.mInterval contained in mInterval?
			INTERVAL_CONTAINED( this, n, false );
			
			CHECK_ORDER();
			n->refine();
			CHECK_ORDER();
			this->refine();
			CHECK_ORDER();
			
			// case: is mInterval contained in o.mInterval?
			INTERVAL_CONTAINED( n, this->thisPtr(), true );
			
			CHECK_ORDER();
			n->refine();
			CHECK_ORDER();
			this->refine();
			CHECK_ORDER();

			Interval<Number> intersection = this->getInterval().intersect(n->getInterval());
			
			// invariant: intersection is nonempty and one bound belongs to an interval
			assert( !intersection.isEmpty() );
			
			// situation normal (not executed if situation is twisted)
			if (this->checkIntersection(n, intersection)) return true;
			// situation twisted
			if (n->checkIntersection(this->thisPtr(), intersection)) return false;
			
			n->refine();
			CHECK_ORDER();
			this->refine();
		}
*/
	}
}
