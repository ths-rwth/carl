#pragma once

namespace carl {
	
	template<typename Number>
	bool RealAlgebraicNumber<Number>::IntervalContent::refineAvoiding(const Number& n, const RealAlgebraicNumber<Number>& parent) {
		assert(!parent.isNumeric());
		
		if (interval.contains(n)) {
			if (polynomial.isRoot(n)) {
				parent.switchToNR(n);
				return true;
			}
			if (polynomial.countRealRoots(Interval<Number>(interval.lower(), BoundType::STRICT, n, BoundType::STRICT)) > 0) {
				interval.setUpper(n);
			} else {
				interval.setLower(n);
			}
			refinementCount++;
		} else if (interval.lower() != n && interval.upper() != n) {
			return false;
		}
		
		bool isLeft = interval.lower() == n;
		
		Number newBound = interval.sample();
		
		if (polynomial.isRoot(newBound)) {
			parent.switchToNR(newBound);
			return false;
		}
		
		if (isLeft) {
			interval.setLower(newBound);
		} else {
			interval.setUpper(newBound);
		}
		
		while (polynomial.countRealRoots(interval) == 0) {
			if (isLeft) {
				Number oldBound = interval.lower();
				newBound = Interval<Number>(n, BoundType::STRICT, oldBound, BoundType::STRICT).sample();
				if (polynomial.isRoot(newBound)) {
					parent.switchToNR(newBound);
					return false;
				}
				interval.setUpper(oldBound);
				interval.setLower(newBound);
			} else {
				Number oldBound = interval.upper();
				newBound = Interval<Number>(oldBound, BoundType::STRICT, n, BoundType::STRICT).sample();
				if (polynomial.isRoot(newBound)) {
					parent.switchToNR(newBound);
					return false;
				}
				interval.setLower(oldBound);
				interval.setUpper(newBound);
			}
		}
		return false;
	}
	
	template<typename Number>
	bool RealAlgebraicNumber<Number>::isIntegral() const {
		while (!isNumeric() && mIR->interval.containsInteger()) {
			refine();
		}
		if (isNumeric()) return carl::isInteger(mValue);
		return false;
	}
	
	template<typename Number>
	void RealAlgebraicNumber<Number>::refine(RealAlgebraicNumberSettings::RefinementStrategy strategy) const {
		if (isNumeric()) return;
		
		Number m;
		switch (strategy) {
			case RealAlgebraicNumberSettings::RefinementStrategy::GENERIC:
				m = mIR->interval.center();
				break;
			case RealAlgebraicNumberSettings::RefinementStrategy::BINARYSAMPLE:
				m = mIR->interval.sample();
				break;
			case RealAlgebraicNumberSettings::RefinementStrategy::BINARYNEWTON:
				m = mIR->interval.sample();
				break;
		}
		assert(mIR->interval.contains(m));
		if (mIR->polynomial.isRoot(m)) {
			switchToNR(m);
		} else {
			if (mIR->polynomial.countRealRoots(Interval<Number>(mIR->interval.lower(), BoundType::STRICT, m, BoundType::STRICT)) > 0) {
				mIR->interval.setUpper(m);
			} else {
				mIR->interval.setLower(m);
			}
			mIR->refinementCount++;
			assert(mIR->interval.isConsistent());
		}
	}
	
	template<typename Number>
	bool RealAlgebraicNumber<Number>::equal(const RealAlgebraicNumber<Number>& n) const {
		CARL_LOG_FUNC("carl.ran", *this << ", " << n);
		if (this == &n) return true;
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
			if (containedIn(n.getInterval())) {
				return true;
			}
			if (n.containedIn(getInterval())) {
				return true;
			}
		}
		
		CARL_LOG_TRACE("carl.ran", "\tNot trivially equal or different");

		if (getPolynomial() != n.getPolynomial()) {
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
			} else {
				n.refineAvoiding(value());
				if (n.isNumeric()) {
					return value() < n.value();
				} else {
					return value() <= n.lower();
				}
			}
		} else {
			if (n.isNumeric()) {
				refineAvoiding(n.value());
				if (isNumeric()) {
					return value() < n.value();
				} else {
					return upper() <= n.value();
				}
			}
		}
		
		if (mIR == n.mIR) return false;
		if (upper() <= n.lower()) return true;
		if (lower() >= n.upper()) return false;
		if (getPolynomial() == n.getPolynomial()) {
			if (containedIn(n.getInterval())) {
				return false;
			}
			if (n.containedIn(getInterval())) {
				return false;
			}
		}
		if (equal(n)) return false;
		
		refine();
		n.refine();
		return less(n);
	}
	
}
