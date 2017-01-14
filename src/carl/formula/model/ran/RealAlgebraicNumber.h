#pragma once

#include <iostream>
#include <memory>

#include "../../../core/Sign.h"
#include "../../../core/UnivariatePolynomial.h"
#include "../../../interval/Interval.h"

#include "RealAlgebraicNumberSettings.h"

#include "../../../thom/ThomEncoding.h"
#include "RealAlgebraicNumber_Interval.h"

namespace carl {

template<typename Number>
class RealAlgebraicNumber {
private:
	friend std::hash<RealAlgebraicNumber<Number>>;
	template<typename Num>
	friend std::ostream& operator<<(std::ostream&, const RealAlgebraicNumber<Num>&);
	using IntervalContent = ran::IntervalContent<Number>;
	using Polynomial = typename IntervalContent::Polynomial;
	
	mutable Number mValue = carl::constant_zero<Number>::get();
	bool mIsRoot = true;
	mutable std::shared_ptr<IntervalContent> mIR;
	std::shared_ptr<ThomEncoding<Number>> mTE;
	
	void checkForSimplification() const {
		if (mIR && mIR->interval.isPointInterval()) {
			switchToNR(mIR->interval.lower());
		}
	}
	
	void switchToNR(const Number& n) const {
		mValue = n;
		if (mIR) {
			mIR->interval = Interval<Number>(n);
			mIR.reset();
		}
	}
	
public:
	RealAlgebraicNumber():
		mValue(carl::constant_zero<Number>::get()),
		mIsRoot(true),
		mIR(),
		mTE()
	{}
	explicit RealAlgebraicNumber(const Number& n, bool isRoot = true):
		mValue(n),
		mIsRoot(isRoot),
		mIR(),
		mTE()
	{}
	explicit RealAlgebraicNumber(Variable var, bool isRoot = true):
		mValue(carl::constant_zero<Number>::get()),
		mIsRoot(isRoot),
		mIR(std::make_shared<IntervalContent>(Polynomial(var), Interval<Number>::zeroInterval())),
		mTE(nullptr)
	{}
	explicit RealAlgebraicNumber(const Polynomial& p, const Interval<Number>& i, bool isRoot = true):
		mValue(carl::constant_zero<Number>::get()),
		mIsRoot(isRoot),
		mIR(std::make_shared<IntervalContent>(p.normalized(), i)),
		mTE(nullptr)
	{
		assert(!mIR->polynomial.isZero() && mIR->polynomial.degree() > 0);
		assert(i.isOpenInterval() || i.isPointInterval());
		assert(p.countRealRoots(i) == 1);
		if (mIR->polynomial.degree() == 1) {
			Number a = mIR->polynomial.coefficients()[1];
			Number b = mIR->polynomial.coefficients()[0];
			switchToNR(-b / a);
		} else {
			if (i.contains(0)) refineAvoiding(0);
		}
	}

	explicit RealAlgebraicNumber(const ThomEncoding<Number>& te, bool isRoot = true):
		mValue(carl::constant_zero<Number>::get()),
		mIsRoot(isRoot),
		mIR(nullptr),
		mTE(std::make_shared<ThomEncoding<Number>>(te))
	{}
		
	RealAlgebraicNumber(const RealAlgebraicNumber& ran) = default;
	RealAlgebraicNumber(RealAlgebraicNumber&& ran) = default;
		
	RealAlgebraicNumber& operator=(const RealAlgebraicNumber& n) = default;
	RealAlgebraicNumber& operator=(RealAlgebraicNumber&& n) noexcept = default;
	
	std::size_t size() const {
		if (isNumeric()) return carl::bitsize(mValue);
		else if (isInterval()) return carl::bitsize(mIR->interval.lower()) + carl::bitsize(mIR->interval.upper()) * mIR->polynomial.degree();
		else return 0;
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
	void setIsRoot(bool isRoot) noexcept {
		mIsRoot = isRoot;
	}
	
	bool isZero() const {
		if (isNumeric()) return carl::isZero(mValue);
		else if (isInterval()) return mIR->interval.isZero();
		else if (isThom()) return mTE->isZero();
		else return false;
	}
	
	bool isNumeric() const {
		checkForSimplification();
		return !mIR && !mTE;
	}
	bool isInterval() const {
		checkForSimplification();
		return bool(mIR);
	}
	bool isThom() const noexcept {
		return bool(mTE);
	}
	const ThomEncoding<Number>& getThomEncoding() const {
		assert(isThom());
		return *mTE;
	}

	
	bool isIntegral() const {
		refineToIntegrality();
		if (isNumeric()) return carl::isInteger(mValue);
		else if (isInterval()) return mIR->isIntegral();
		else return false;
	}
	
	Number branchingPoint() const {
		if (isNumeric()) return mValue;
		assert(isInterval());
		return mIR->interval.sample();
	}
	
	const Number& value() const noexcept {
		assert(isNumeric());
		return mValue;
	}
	
	std::size_t getRefinementCount() const {
		assert(!isNumeric() && !isThom());
		assert(isInterval());
		return mIR->refinementCount;
	}
	const Interval<Number>& getInterval() const {
		assert(isInterval());
		return mIR->interval;
	}
	const Number& lower() const {
		return getInterval().lower();
	}
	const Number& upper() const {
		return getInterval().upper();
	}
	const Polynomial& getIRPolynomial() const {
		assert(!isNumeric());
		assert(isInterval());
		return mIR->polynomial;
	}
	
	RealAlgebraicNumber changeVariable(Variable::Arg v) const {
		if (isNumeric()) return *this;
		assert(isInterval());
		return RealAlgebraicNumber<Number>(mIR->polynomial.replaceVariable(v), mIR->interval, mIsRoot);
	}
	
	Sign sgn() const {
		if (isNumeric()) {
			return carl::sgn(mValue);
		}
		else if (isInterval()) {
			return mIR->interval.sgn();
		}
		else if(isThom()) {
			return mTE->sgnReprNum();
		}
		else return Sign::ZERO;
	}
	
	Sign sgn(const Polynomial& p) const {
		if (isNumeric()) {
			return carl::sgn(p.evaluate(mValue));
		} else if (isInterval()){
			return mIR->sgn(p);
		} else {
			assert(isThom());
			return mTE->signOnPolynomial(MultivariatePolynomial<Number>(p));
		}
	}
	
	bool isRootOf(const UnivariatePolynomial<Number>& p) const {
		if (isNumeric()) return p.countRealRoots(value()) == 1;
		else if (isInterval()) return p.countRealRoots(mIR->interval) == 1;
		else if (isThom()) return this->sgn(p) == Sign::ZERO;
		else return false;
	}
	
	bool containedIn(const Interval<Number>& i) const {
		if (isNumeric()) return i.contains(mValue);
		else if (isInterval()) {
			if (mIR->interval.contains(i.lower())) {
				refineAvoiding(i.lower());
				if (isNumeric()) return i.contains(mValue);
			}
			if (mIR->interval.contains(i.upper())) {
				refineAvoiding(i.upper());
				if (isNumeric()) return i.contains(mValue);
			}
			return i.contains(mIR->interval);
		} else if (isThom()) {
			if(i.lowerBoundType() != BoundType::INFTY) {
				if(i.lowerBoundType() == BoundType::STRICT && *mTE <= i.lower()) return false;
				if(i.lowerBoundType() == BoundType::WEAK && *mTE < i.lower()) return false;
			}
			if(i.upperBoundType() != BoundType::INFTY) {
				if(i.upperBoundType() == BoundType::STRICT && *mTE >= i.upper()) return false;
				if(i.upperBoundType() == BoundType::WEAK && *mTE > i.upper()) return false;
			}
			return true;
		}
		else return false;
	}
	
	bool refineAvoiding(const Number& n) const {
		assert(!isNumeric());
		bool res = mIR->refineAvoiding(n);
		checkForSimplification();
		return res;
	}
	/// Refines until the number is either numeric or the interval does not contain any integer.
	void refineToIntegrality() const {
		if (isInterval()) mIR->refineToIntegrality();
		checkForSimplification();
	}
	void refine() const {
		if (isInterval()) mIR->refine();
		checkForSimplification();
	}
	
	RealAlgebraicNumber<Number> abs() const {
		if (isNumeric()) return RealAlgebraicNumber<Number>(carl::abs(mValue), mIsRoot);
		if (isInterval()) return RealAlgebraicNumber<Number>(mIR->polynomial, mIR->interval.abs(), mIsRoot);
		return RealAlgebraicNumber<Number>();
	}

	bool equal(const RealAlgebraicNumber<Number>& n) const;
	bool less(const RealAlgebraicNumber<Number>& n) const;
	std::pair<bool,bool> checkOrder(const RealAlgebraicNumber<Number>& n) const;
private:
	bool lessWhileUnequal(const RealAlgebraicNumber<Number>& n) const;
public:
	static RealAlgebraicNumber<Number> sampleBelow(const RealAlgebraicNumber<Number>& n);
	static RealAlgebraicNumber<Number> sampleBetween(const RealAlgebraicNumber<Number>& lower, const RealAlgebraicNumber<Number>& upper);
	static RealAlgebraicNumber<Number> sampleAbove(const RealAlgebraicNumber<Number>& n);
};

template<typename Num>
std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumber<Num>& ran) {
	if (ran.isNumeric()) return os << "(NR " << ran.value() << (ran.isRoot() ? " R" : "") << ")";
	else if (ran.isInterval()) return os << "(IR " << ran.getInterval() << ", " << ran.getIRPolynomial() << (ran.isRoot() ? " R" : "") << ")";
	else if (ran.isThom()) return os << "(TE " << ran.getThomEncoding() << (ran.isRoot() ? " R" : "") << ")";
	else return os << "(RAN)";
}

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
