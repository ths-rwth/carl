
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
		if (getPolynomial() == n.getPolynomial()) {
			if (n.lower() <= lower()) {
				if (upper() <= n.upper()) return true;
				refineAvoiding(n.upper());
			} else {
				assert(lower() <= n.lower());
				if (n.upper() <= upper()) return true;
				refineAvoiding(n.lower());
			}
		} else {
			assert(getPolynomial() != n.getPolynomial());
			auto g = UnivariatePolynomial<Number>::gcd(getPolynomial(), n.getPolynomial());
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
	
	template<typename Number>
	RealAlgebraicNumber<Number> RealAlgebraicNumber<Number>::sampleBelow(const RealAlgebraicNumber<Number>& n) {
		CARL_LOG_FUNC("carl.ran", n);
		if (n.isNumeric()) {
			CARL_LOG_TRACE("carl.ran", "Selecting from (-oo, " << n << ") -> " << (carl::ceil(n.value()) - 1));
			return RealAlgebraicNumber<Number>(carl::ceil(n.value()) - 1, false);
		} else if (n.isInterval()) {
			CARL_LOG_TRACE("carl.ran", "Selecting from (-oo, " << n << ") -> " << (carl::ceil(n.lower()) - 1));
			return RealAlgebraicNumber<Number>(carl::ceil(n.lower()) - 1, false);
		} else {
			assert(n.isThom());
			RealAlgebraicNumber<Number> res(*(n.mTE) + Number(-1), false);
			CARL_LOG_TRACE("carl.ran", "selecting sample " << res);
			return res;
		}
	}
	template<typename Number>
	RealAlgebraicNumber<Number> RealAlgebraicNumber<Number>::sampleBetween(const RealAlgebraicNumber<Number>& lower, const RealAlgebraicNumber<Number>& upper) {
		CARL_LOG_FUNC("carl.ran", lower << ", " << upper);
		if(lower.isThom() || upper.isThom()) {
			RealAlgebraicNumber<Number> res;
			if(lower.isThom() && upper.isThom()) {
				res =  RealAlgebraicNumber<Number>(ThomEncoding<Number>::intermediatePoint(*(lower.mTE), *(upper.mTE)), false);
			} else if(lower.isNumeric() && upper.isThom()) {
				res = RealAlgebraicNumber<Number>(ThomEncoding<Number>::intermediatePoint(lower.value(), *(upper.mTE)), false);
			} else {
				assert(lower.isThom() && upper.isNumeric());
				res = RealAlgebraicNumber<Number>(ThomEncoding<Number>::intermediatePoint(*(lower.mTE), upper.value()), false);
			}
			CARL_LOG_TRACE("carl.ran", "selecting sample " << res);
			return res;
		} else {
			carl::Interval<Number> i;
			if (lower.isNumeric()) i.set(lower.value(), lower.value());
			else i.set(lower.upper(), lower.upper());
			if (upper.isNumeric()) i.setUpper(upper.value());
			else i.setUpper(upper.lower());
			while (i.isEmpty()) {
				if (!lower.isNumeric()) {
					lower.refine();
					if (lower.isNumeric()) i.setLower(lower.value());
					else i.setLower(lower.upper());
				}
				if (!upper.isNumeric()) {
					upper.refine();
					if (upper.isNumeric()) i.setUpper(upper.value());
					else i.setUpper(upper.lower());
				}
			}
			CARL_LOG_TRACE("carl.ran", "Selecting from (" << lower << ", " << upper << ") -> " << i.sample(false) << " (from " << i << ")");
			return RealAlgebraicNumber<Number>(i.sample(false), false);
		}
	}
	template<typename Number>
	RealAlgebraicNumber<Number> RealAlgebraicNumber<Number>::sampleAbove(const RealAlgebraicNumber<Number>& n) {
		CARL_LOG_FUNC("carl.ran", n);
		if (n.isNumeric()) {
			CARL_LOG_TRACE("carl.ran", "Selecting from (" << n << ", oo) -> " << (carl::floor(n.value()) + 1));
			return RealAlgebraicNumber<Number>(carl::floor(n.value()) + 1, false);
		} else if (n.isInterval()) {
			CARL_LOG_TRACE("carl.ran", "Selecting from (" << n << ", oo) -> " << (carl::floor(n.upper()) + 1));
			return RealAlgebraicNumber<Number>(carl::floor(n.upper()) + 1, false);
		} else {
			assert(n.isThom());
			RealAlgebraicNumber<Number> res(*(n.mTE) + Number(1), false);
			CARL_LOG_TRACE("carl.ran", "selecting sample " << res);
			return res;
		}
	}
}
