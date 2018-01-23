
#include "RealAlgebraicNumber.h"

#pragma once

namespace carl {
	template<typename Number>
	bool RealAlgebraicNumber<Number>::equal(const RealAlgebraicNumber<Number>& n) const {
		CARL_LOG_FUNC("carl.ran", *this << ", " << n);
		if (this == &n) return true;

		if(this->isThom() || n.isThom()) {
			assert(!this->isInterval() && !n.isInterval());
			if(this->isThom() && n.isThom()) {
				return *(this->mTE) == *(n.mTE);
			} else if(this->isThom() && n.isNumeric()) {
				return *(this->mTE) == n.value();
			} else {
				assert(this->isNumeric() && n.isThom());
				return this->value() == *(n.mTE);
			}
		}
		assert(!isThom() && !n.isThom());

		if (isNumeric()) {
			if (n.isNumeric()) {
				return value() == n.value();
			} else {
				return n.refineAvoiding(value());
			}
		} else {
			if (n.isNumeric()) {
				return refineAvoiding(n.value());
			}
		}
		if (mIR == n.mIR) return true;
		if (upper() <= n.lower()) return false;
		if (lower() >= n.upper()) return false;
		if (getIRPolynomial() == n.getIRPolynomial()) {
			if (n.lower() <= lower()) {
				if (upper() <= n.upper()) return true;
				refineAvoiding(n.upper());
			} else {
				assert(lower() <= n.lower());
				if (n.upper() <= upper()) return true;
				refineAvoiding(n.lower());
			}
		} else {
			assert(getIRPolynomial() != n.getIRPolynomial());
			assert(getIRPolynomial().mainVar() == n.getIRPolynomial().mainVar());
			auto g = UnivariatePolynomial<Number>::gcd(getIRPolynomial(), n.getIRPolynomial());
			if (!isRootOf(g)) return false;
			mIR->polynomial = g;
			mIR->sturmSequence = g.standardSturmSequence();
			if (!n.isRootOf(g)) return false;
			n.mIR->polynomial = g;
			n.mIR->sturmSequence = mIR->sturmSequence;
			return equal(n);
		}
		return equal(n);
	}
	
	template<typename Number>
	bool RealAlgebraicNumber<Number>::less(const RealAlgebraicNumber<Number>& n) const {
		CARL_LOG_FUNC("carl.ran", *this << ", " << n);
		if (this == &n) return false;
		if (isNumeric()) {
			if (n.isNumeric()) {
				return value() < n.value();
			} else if (n.isInterval()) {
				n.refineAvoiding(value());
				if (n.isNumeric()) {
					return value() < n.value();
				} else {
					return value() <= n.lower();
				}
			} else {
				assert(n.isThom());
				bool res = this->value() < *(n.mTE);
				CARL_LOG_TRACE("carl.ran", "result is " << res);
				return res;
			}
		} else if(isInterval()) {
			if (n.isNumeric()) {
				refineAvoiding(n.value());
				if (isNumeric()) {
					return value() < n.value();
				} else {
					return upper() <= n.value();
				}
			}
		} else {
			assert(isThom());
			if(n.isNumeric()) {
				bool res = *mTE < n.value();
				CARL_LOG_TRACE("carl.ran", "result is " << res);
				return res;
			} else {
				assert(n.isThom());
				bool res =  *mTE < *(n.mTE);
				CARL_LOG_TRACE("carl.ran", "result is " << res);
				return res;
			}
		}

		if (mIR == n.mIR) return false;
		if (upper() <= n.lower()) return true;
		if (lower() >= n.upper()) return false;
		if (equal(n)) return false;
		return lessWhileUnequal(n);
	}
	
	template<typename Number>
	std::pair<bool,bool> RealAlgebraicNumber<Number>::checkOrder(const RealAlgebraicNumber<Number>& n) const {
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
		assert(!equal(n));
		
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
