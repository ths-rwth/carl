#pragma once

#include <iostream>
#include <memory>

#include "UnivariatePolynomial.h"
#include "../interval/Interval.h"
#include "../core/Sign.h"

#include "RealAlgebraicNumberSettings.h"

namespace carl {

template<typename Number>
class RealAlgebraicNumber {
private:
	friend std::hash<RealAlgebraicNumber<Number>>;
	using Polynomial = UnivariatePolynomial<Number>;
	struct IntervalContent {
		Polynomial polynomial;
		Interval<Number> interval;
		std::list<Polynomial> sturmSequence;
		std::size_t refinementCount;
		
		IntervalContent(
			const Polynomial& p,
			const Interval<Number> i
		):
			polynomial(p),
			interval(i),
			sturmSequence(p.standardSturmSequence()),
			refinementCount(0)
		{}
		
		IntervalContent(
			const Polynomial& p,
			const Interval<Number> i,
			const std::list<UnivariatePolynomial<Number>>& seq
		):
			polynomial(p),
			interval(i),
			sturmSequence(seq),
			refinementCount(0)
		{}
			
		/** Refines the interval i of this real algebraic number yielding the interval j such that !j.meets(n). If true is returned, n is the exact numeric representation of this root. Otherwise not.
		 * @param n
		 * @rcomplexity constant
		 * @scomplexity constant
		 * @return true, if n is the exact numeric representation of this root, otherwise false
		 */
		bool refineAvoiding(const Number& n, const RealAlgebraicNumber<Number>& parent);
	};
	
	mutable Number mValue;
	bool mIsRoot;
	mutable std::shared_ptr<IntervalContent> mIR;
	
	void checkForSimplification() const {
		if (mIR && mIR->interval.isPointInterval()) {
			switchToNR(mIR->interval.lower());
		}
	}
	
	void switchToNR(const Number& n) const {
		assert(mIR);
		mIR->interval = Interval<Number>(n);
		mValue = n;
		mIR.reset();
	}
	
public:
	RealAlgebraicNumber():
		mValue(carl::constant_zero<Number>::get()),
		mIsRoot(true)
	{}
	explicit RealAlgebraicNumber(const Number& n, bool isRoot = true):
		mValue(n),
		mIsRoot(isRoot)
	{}
	explicit RealAlgebraicNumber(Variable::Arg var, bool isRoot = true):
		mValue(carl::constant_zero<Number>::get()),
		mIsRoot(isRoot),
		mIR(std::make_shared<IntervalContent>(Polynomial(var), Interval<Number>::zeroInterval()))
	{}
	explicit RealAlgebraicNumber(const Polynomial& p, const Interval<Number>& i, bool isRoot = true):
		mValue(carl::constant_zero<Number>::get()),
		mIsRoot(isRoot),
		mIR(std::make_shared<IntervalContent>(p.normalized(), i))
	{
		assert(!mIR->polynomial.isZero() && mIR->polynomial.degree() > 0);
		assert(i.isOpenInterval() || i.isPointInterval());
		assert(p.countRealRoots(i) == 1);
		if (mIR->polynomial.degree() == 1) {
			Number a = mIR->polynomial.coefficients()[1];
			Number b = mIR->polynomial.coefficients()[0];
			switchToNR(-b / a);
		} else {
			if (i.contains(0)) mIR->refineAvoiding(0, *this);
		}
	}
		
	RealAlgebraicNumber(const RealAlgebraicNumber& ran):
		mValue(ran.mValue),
		mIsRoot(ran.mIsRoot),
		mIR(ran.mIR)
	{}
	RealAlgebraicNumber(RealAlgebraicNumber&& ran):
		mValue(std::move(ran.mValue)),
		mIsRoot(ran.mIsRoot),
		mIR(std::move(ran.mIR))
	{}
		
	RealAlgebraicNumber& operator=(const RealAlgebraicNumber& n) {
		mValue = n.mValue;
		mIsRoot = n.mIsRoot;
		mIR = n.mIR;
		return *this;
	}
	RealAlgebraicNumber& operator=(RealAlgebraicNumber&& n) {
		mValue = std::move(n.mValue);
		mIsRoot = n.mIsRoot;
		mIR = std::move(n.mIR);
		return *this;
	}
	
	/**
	 * @return the flag marking whether the real algebraic number stems from a root computation or not
	 */
	bool isRoot() const {
		return mIsRoot;
	}

	/**
	 * Set the flag marking whether the real algebraic number stems from a root computation or not.
	 * @param isRoot
	 */
	void setIsRoot(bool isRoot) {
		mIsRoot = isRoot;
	}
	
	bool isZero() const {
		if (isNumeric()) return carl::isZero(mValue);
		else return mIR->interval.isZero();
	}
	
	bool isNumeric() const {
		checkForSimplification();
		return !mIR;
	}
	
	bool isIntegral() const;
	
	Number branchingPoint() const {
		if (isNumeric()) return mValue;
		else return mIR->interval.sample();
	}
	
	const Number& value() const {
		assert(isNumeric());
		return mValue;
	}
	
	std::size_t getRefinementCount() const {
		assert(!isNumeric());
		return mIR->refinementCount;
	}
	const Interval<Number>& getInterval() const {
		assert(!isNumeric());
		return mIR->interval;
	}
	const Number& lower() const {
		return getInterval().lower();
	}
	const Number& upper() const {
		return getInterval().upper();
	}
	const Polynomial& getPolynomial() const {
		assert(!isNumeric());
		return mIR->polynomial;
	}
	
	Sign sgn() const {
		if (isNumeric()) {
			return carl::sgn(mValue);
		} else {
			return mIR->interval.sgn();
		}
	}
	
	Sign sgn(const Polynomial& p) const {
		if (isNumeric()) {
			return carl::sgn(p.evaluate(mValue));
		} else {
			if (mIR->polynomial == p) return Sign::ZERO;
			auto seq = mIR->polynomial.standardSturmSequence(mIR->polynomial.derivative() * p);
			int variations = Polynomial::countRealRoots(seq, mIR->interval);
			assert((variations == -1) || (variations == 0) || (variations == 1));
			switch (variations) {
				case -1: return Sign::NEGATIVE;
				case 0: return Sign::ZERO;
				case 1: return Sign::POSITIVE;
			}
			CARL_LOG_ERROR("carl.ran", "Unexpected number of variations, should be -1, 0, 1 but was " << variations);
			return Sign::ZERO;
		}
	}
	
	bool isRootOf(const UnivariatePolynomial<Number>& p) const {
		if (isNumeric()) return p.countRealRoots(value()) == 1;
		else return p.countRealRoots(getInterval()) == 1;
	}
	
	bool containedIn(const Interval<Number>& i) const {
		if (isNumeric()) return i.contains(mValue);
		else {
			if (getInterval().contains(i.lower())) {
				mIR->refineAvoiding(i.lower(), *this);
				if (isNumeric()) return i.contains(mValue);
			}
			if (getInterval().contains(i.upper())) {
				mIR->refineAvoiding(i.upper(), *this);
				if (isNumeric()) return i.contains(mValue);
			}
			return i.contains(mIR->interval);
		}
	}
	
	bool refineAvoiding(const Number& n) const {
		assert(!isNumeric());
		return mIR->refineAvoiding(n, *this);
	}
	void refine(RealAlgebraicNumberSettings::RefinementStrategy strategy = RealAlgebraicNumberSettings::RefinementStrategy::DEFAULT) const;
	/// Refines until the number is either numeric or the interval does not contain any integer.
	void refineToIntegrality() const {
		while (!isNumeric() && mIR->interval.containsInteger()) {
			refine();
		}
	}
	
	bool equal(const RealAlgebraicNumber<Number>& n) const;
	bool less(const RealAlgebraicNumber<Number>& n) const;
	std::pair<bool,bool> checkOrder(const RealAlgebraicNumber<Number>& n) const;
private:
	bool lessWhileUnequal(const RealAlgebraicNumber<Number>& n) const;
public:
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumber<Num>& ran) {
		if (ran.isNumeric()) return os << "(NR " << ran.value() << (ran.isRoot() ? " R" : "") << ")";
		else return os << "(IR " << ran.getInterval() << ", " << ran.getPolynomial() << (ran.isRoot() ? " R" : "") << ")";
	}
};

template<typename Number>
inline bool operator==(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	return lhs.equal(rhs);
}
template<typename Number>
inline bool operator!=(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	return !(lhs == rhs);
}
template<typename Number>
inline bool operator<(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	return lhs.less(rhs);
}
template<typename Number>
inline bool operator<=(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	return !(rhs < lhs);
}
template<typename Number>
inline bool operator>(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	return rhs < lhs;
}
template<typename Number>
inline bool operator>=(const RealAlgebraicNumber<Number>& lhs, const RealAlgebraicNumber<Number>& rhs) {
	return rhs <= lhs;
}

}

namespace std {
	
	template<typename Number>
	struct hash<carl::RealAlgebraicNumber<Number>> {
		std::size_t operator()(const carl::RealAlgebraicNumber<Number>& n) const {
			if (n.isNumeric()) {
				return carl::hash_all(true, n.mIsRoot, n.mValue);
			} else {
				return carl::hash_all(false, n.mIsRoot, n.mIR);
			}
		}
	};
	
}

#include "RealAlgebraicNumberOperations.h"
#include "RealAlgebraicNumber.tpp"
