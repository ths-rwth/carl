/*
 * File:   RealAlgebraicNumberIR.tpp
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../util/SFINAE.h"
#include "UnivariatePolynomial.h"
#include "RealAlgebraicNumberIR.h"
#include "RealAlgebraicNumber.h"
#include "rootfinder/RootFinder.h"

namespace carl {

template<typename Number>
RealAlgebraicNumberIR<Number>::RealAlgebraicNumberIR(const Variable& var) :
		RealAlgebraicNumber<Number>(true, true, 0),
		polynomial(var),
		interval(Interval<Number>::zeroInterval()),
		sturmSequence(this->polynomial.standardSturmSequence()),
		refinementCount(0)
{
}

template<typename Number>
RealAlgebraicNumberIR<Number>::RealAlgebraicNumberIR(
		const UnivariatePolynomial<Number>& p,
		const Interval<Number>& i,
		const std::list<UnivariatePolynomial<Number>>& seq,
		const bool normalize,
		const bool isRoot ) :
		RealAlgebraicNumber<Number>(isRoot, false, 0),
		//polynomial(p.squareFreePart().template convert<Number>()),
		polynomial(p),
		interval(i),
		sturmSequence( seq.empty() ? polynomial.standardSturmSequence() : seq ),
		refinementCount(0)
{
	assert(!this->polynomial.isConstant());
	assert(p.countRealRoots(i) == 1);
	if (normalize) this->normalizeInterval();
	if (this->interval.contains(0)) {
		this->mIsNumeric = true;
	}
	if (this->polynomial.degree() <= 1) {
		Number a = this->polynomial.coefficients()[1];
		Number b = this->polynomial.coefficients()[0];
		this->mValue = (a == 0) ? b : (-b/a);
		this->mIsNumeric = true;
		this->interval.setLower(Interval<Number>(this->interval.lower(), BoundType::STRICT, this->value(), BoundType::STRICT).sample());
		this->interval.setUpper(Interval<Number>(this->value(), BoundType::STRICT, this->interval.upper(), BoundType::STRICT).sample());
	}
	assert(p.countRealRoots(i) == 1);
}

template<typename Number>
RealAlgebraicNumberIRPtr<Number> RealAlgebraicNumberIR<Number>::add(const RealAlgebraicNumberIRPtr<Number>& n) {
	if (this->isZero() || n->isZero()) return n;

	Variable va = this->getPolynomial().mainVar();
	Variable y = VariablePool::getInstance().getFreshVariable();

	MultivariatePolynomial<Number> tmp1(this->getPolynomial());
	tmp1 = tmp1.substitute(va, MultivariatePolynomial<Number>({Term<Number>(va), -Term<Number>(y)}));
	MultivariatePolynomial<Number> tmp2(n->getPolynomial().replaceVariable(y));
	UnivariatePolynomial<Number> res(tmp1.toUnivariatePolynomial(y).resultant(tmp2.toUnivariatePolynomial(y)).switchVariable(va).toNumberCoefficients());
	
	auto p = res.switchVariable(va).coprimeCoefficients().primitivePart().template convert<Number>();
	auto seq = p.standardSturmSequence();

	Interval<Number> i = this->getInterval() + n->getInterval();
	while (p.isRoot(i.lower()) || p.isRoot(i.upper()) || UnivariatePolynomial<Number>::countRealRoots(seq, i) > 1) {
		this->refine();
		n->refine();
		i = this->getInterval() + n->getInterval();
	}
	return RealAlgebraicNumberIR<Number>::create(p, i, seq);
}

template<typename Number>
std::shared_ptr<RealAlgebraicNumberIR<Number>> RealAlgebraicNumberIR<Number>::minus() const {
	if (this->isZero()) {
		return RealAlgebraicNumberIR<Number>::create(this->polynomial, this->interval, this->sturmSequence);
	}
	return RealAlgebraicNumberIR<Number>::create(this->polynomial.negateVariable(), this->interval.inverse());
}

template<typename Number>
bool RealAlgebraicNumberIR<Number>::equal(RealAlgebraicNumberIRPtr<Number>& n) {
	if (this == n.get()) return true;
	if (n.get() == nullptr) return false;
	if (this->isZero() && n->isZero()) return true;
	if (this->upper() <= n->lower()) return false;
	if (this->lower() >= n->upper()) return false;
	if ((this->interval == n->interval) && (this->polynomial == n->polynomial)) {
		n = this->thisPtr();
		return true;
	}
	return this->add(n->minus())->isZero();
}

template<typename Number>
std::pair<bool,bool> RealAlgebraicNumberIR<Number>::checkOrder(RealAlgebraicNumberIRPtr<Number> n) {
	if (this->isNumeric()) {
		if (n->isNumeric()) {
			return std::make_pair(true, this->value() < n->value());
		} else {
			n->refineAvoiding(this->value());
			if (this->value() <= n->lower()) {
				this->setUpper(n->upper());
				return std::make_pair(true, true);
			} else {
				assert(this->value() >= n->upper());
				this->setUpper(n->upper());
				return std::make_pair(true, false);
			}
		}
	}
	if (n->isNumeric()) {
		this->refineAvoiding(n->value());
		if (n->value() <= this->lower()) {
			n->setUpper(this->lower());
			return std::make_pair(true, false);
		} else {
			assert(n->value() >= this->upper());
			n->setLower(this->upper());
			return std::make_pair(true, true);
		}
	}
	
	if (this->upper() <= n->lower()) {
		return std::make_pair(true, true);
	}
	if (n->upper() <= this->lower()) {
		return std::make_pair(true, false);
	}
	
	return std::make_pair(false, false);
}

template<typename Number>
std::pair<bool,bool> RealAlgebraicNumberIR<Number>::intervalContained(RealAlgebraicNumberIRPtr<Number> n, bool twisted) {
	if (this->getInterval().contains(n->getInterval())) {
		if (this->getPolynomial().isRoot(n->lower())) {
			this->mValue = n->lower();
			this->mIsNumeric = true;
			n->refineAvoiding(this->value());
			this->setUpper(n->lower());
			return std::make_pair(true, !twisted);
		} else if (this->getPolynomial().isRoot(n->upper())) {
			this->mValue = n->upper();
			this->mIsNumeric = true;
			n->refineAvoiding(this->value());
			this->setLower(n->upper());
			return std::make_pair(true, twisted);
		} else if (this->getPolynomial().countRealRoots(n->getInterval()) == 0) {
			if (this->lower() != n->lower() && this->getPolynomial().countRealRoots(Interval<Number>(this->lower(), BoundType::STRICT, n->lower(), BoundType::STRICT)) > 0) {
				this->setUpper(n->lower());
				return std::make_pair(true, !twisted);
			} else if (this->upper() != n->upper() && this->getPolynomial().countRealRoots(Interval<Number>(n->upper(), BoundType::STRICT, this->upper(), BoundType::STRICT)) > 0) {
				this->setLower(n->upper());
				return std::make_pair(true, twisted);
			}
			assert(this->getInterval() == n->getInterval());
		} else {
			this->setLower(n->lower());
			this->setUpper(n->upper());
		}
	}
	return std::make_pair(false, false);
}

template<typename Number>
bool RealAlgebraicNumberIR<Number>::checkIntersection(RealAlgebraicNumberIRPtr<Number> n, const Interval<Number> i) {
	// Proceed only if this.left < n2.left and this.right < n2.right
	if ((this->lower() < n->lower()) && (this->upper() < n->upper())) {
		assert( i.lower() == n->lower() && i.upper() == this->upper());
		if (this->getPolynomial().isRoot(i.lower())) {
			// If i.left is root of n1.polynomial: convert n1 to NR
			this->mValue = i.lower();
			this->mIsNumeric = true;
			n->refineAvoiding(this->value());
			this->setUpper(n->lower());
			return true;
		}
		if (this->getPolynomial().countRealRoots(i) == 0) {
			// i contains no roots of n1.polynomial
			this->refineAvoiding(i.lower());
			return true;
		}
		if (n->getPolynomial().isRoot(i.upper())) {
			// If i.right is root of n2.polynomial: convert n2 to NR
			n->mValue = i.upper();
			n->mIsNumeric = true;
			this->refineAvoiding(n->value());
			n->setLower(this->upper());
			return true;
		}
		if (n->getPolynomial().countRealRoots(i) == 0) {
			// i contains no roots of n2.polynomial
			n->refineAvoiding(i.upper());
			return true;
		}
		this->setLower(i.lower());
		n->setUpper(i.upper());
	}
	return false;
}


template<typename Number>
bool RealAlgebraicNumberIR<Number>::lessWhileUnequal(RealAlgebraicNumberIRPtr<Number> n) {
	assert(!this->equal(n));
	if (this->isNumeric() && n->isNumeric()) {
		return this->value() < n->value();
	}
	
	#define CHECK_ORDER() {\
		auto res = this->checkOrder(n);\
		if (res.first) return res.second;\
	}
	#define INTERVAL_CONTAINED(n1, n2, twisted) {\
		auto res = n1->intervalContained(n2, twisted);\
		if (res.first) return res.second;\
	}
	
	while (true) {
		CHECK_ORDER();

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
}

template<typename Number>
void RealAlgebraicNumberIR<Number>::normalizeInterval() {
	if (this->interval.lower() == 0 && this->interval.upper() == 0) return; // already normalized
	if (this->isNumeric()) return;
	assert(this->polynomial.countRealRoots(this->interval) == 1); // the interval should be isolating for this number
	// shift the right border below 0 or set the zero interval
	if (this->interval.contains(0) && this->polynomial.sgn(0) == Sign::ZERO) {
		this->interval.set(0,0);
		this->interval.setLowerBoundType(BoundType::WEAK);
		this->interval.setUpperBoundType(BoundType::WEAK);
		this->mValue = 0;
		this->mIsNumeric = true;
	} else if (this->interval.meets(0)) {
		// one of the bounds might be 0
		// the separation is computed following [p.329,Algorithmic Algebra ISBN 3-540-94090-1]
		Number sep = (Number)1 / (1 + this->polynomial.maximumNorm());
		assert(this->polynomial.sgn(sep) != Sign::ZERO);
		if ((this->interval.upper() == 0) || 
			(this->interval.lower() < -sep && this->polynomial.countRealRoots(Interval<Number>(this->interval.lower(), BoundType::STRICT, -sep, BoundType::STRICT)) > 0)) {
			this->interval.setUpper(-sep);
		} else {
			this->interval.setLower(sep);
		}
	}
	assert( ( !this->interval.meets(0) && this->polynomial.countRealRoots(this->interval) == 1 ) || (this->interval.lower() == 0 && this->interval.upper() == 0)); // otherwise, the interval is not suitable for this real algebraic number
}

template<typename Number>
void RealAlgebraicNumberIR<Number>::coarsen(const Interval<Number>& i) const {
	if (i.lower() < this->interval.lower()) { // coarsen the left bound
		Number l = this->interval.lower();
		this->interval.setLower(i.lower());
		while (this->polynomial.sgn(this->interval.lower()) == Sign::ZERO || countRealRoots(this->sturmSequence, this->interval) != 1) {
			this->interval.setLower(Interval<Number>(this->interval.lower(), l).sampleFast());
		}
	}
	if (i.upper() > this->interval.upper()) { // coarsen the right bound
		Number r = this->interval.upper();
		this->interval.setUpper( interval.upper() );
		while (this->polynomial->sgn(this->interval.upper()) == Sign::ZERO || countRealRoots(this->sturmSequence, this->interval) != 1) {
			this->interval.setUpper(Interval<Number>(r, this->interval.upper()).sampleFast());
		}
	}
}

template<typename Number>
void RealAlgebraicNumberIR<Number>::refine(RealAlgebraicNumberSettings::RefinementStrategy strategy) {
	if (this->isNumeric()) {
		// This RANIR already has a numeric value
		assert(this->lower() != this->value());
		assert(this->upper() != this->value());
		this->setLower(Interval<Number>(this->lower(), BoundType::STRICT, this->value(), BoundType::STRICT).sample());
		this->setUpper(Interval<Number>(this->value(), BoundType::STRICT, this->upper(), BoundType::STRICT).sample());
		return;
	}
	
	Number m;
	switch (strategy) {
		case RealAlgebraicNumberSettings::RefinementStrategy::GENERIC:
			m = this->getInterval().center();
			break;
		case RealAlgebraicNumberSettings::RefinementStrategy::BINARYSAMPLE:
			m = this->getInterval().sample();
			break;
		case RealAlgebraicNumberSettings::RefinementStrategy::BINARYNEWTON:
			m = this->getInterval().sample();
			break;
	}
	assert(this->interval.contains(m));
	if (this->getPolynomial().isRoot(m)) {
		this->setLower(Interval<Number>(this->lower(), BoundType::STRICT, m, BoundType::STRICT).sample());
		this->setUpper(Interval<Number>(m, BoundType::STRICT, this->upper(), BoundType::STRICT).sample());
		this->mValue = m;
		this->mIsNumeric = true;
	} else {
		if (this->getPolynomial().countRealRoots(Interval<Number>(this->lower(), BoundType::STRICT, m, BoundType::STRICT)) > 0) {
			this->setUpper(m);
		} else {
			this->setLower(m);
		}
	}
	
	this->refinementCount++;
	assert(this->lower() < this->upper());
}

template<typename Number>
bool RealAlgebraicNumberIR<Number>::refineAvoiding(const Number& n) {
	if (this->isNumeric()) {
		if (!this->getInterval().meets(n)) return false;
		if (this->value() < n) {
			this->setUpper(Interval<Number>(this->value(), BoundType::STRICT, n, BoundType::STRICT).sample());
		} else if (this->value() > n) {
			this->setLower(Interval<Number>(n, BoundType::STRICT, this->value(), BoundType::STRICT).sample());
		} else return true;
		assert(this->lower() < this->upper());
		return false;
	}
	
	if (this->getInterval().contains(n)) {
		if (this->getPolynomial().isRoot(n)) {
			this->mValue = n;
			this->mIsNumeric = true;
			return true;
		}
		if (this->getPolynomial().countRealRoots(Interval<Number>(this->lower(), BoundType::STRICT, n, BoundType::STRICT)) > 0) {
			this->setUpper(n);
		} else {
			this->setLower(n);
		}
		this->refinementCount++;
	} else if (this->lower() != n && this->upper() != n) {
		return false;
	}
	
	bool isLeft = this->lower() == n;
	
	Number newBound = this->getInterval().sample();
	
	if (this->getPolynomial().isRoot(newBound)) {
		this->mValue = newBound;
		this->mIsNumeric = true;
		if (isLeft) {
			this->setLower(Interval<Number>(n, BoundType::STRICT, newBound, BoundType::STRICT).sample());
		} else {
			this->setUpper(Interval<Number>(newBound, BoundType::STRICT, n, BoundType::STRICT).sample());
		}
		return false;
	}
	
	if (isLeft) {
		this->setLower(newBound);
	} else {
		this->setUpper(newBound);
	}
	
	while (this->getPolynomial().countRealRoots(this->getInterval()) == 0) {
		if (isLeft) {
			Number oldBound = this->lower();
			Number newBound = Interval<Number>(n, BoundType::STRICT, oldBound, BoundType::STRICT).sample();
			if (this->getPolynomial().isRoot(newBound)) {
				this->mValue = newBound;
				this->mIsNumeric = true;
				this->setLower(Interval<Number>(n, BoundType::STRICT, newBound, BoundType::STRICT).sample());
				return false;
			}
			this->setUpper(oldBound);
			this->setLower(newBound);
		} else {
			Number oldBound = this->upper();
			Number newBound = Interval<Number>(oldBound, BoundType::STRICT, n, BoundType::STRICT).sample();
			if (this->getPolynomial().isRoot(newBound)) {
				this->mValue = newBound;
				this->mIsNumeric = true;
				this->setUpper(Interval<Number>(newBound, BoundType::STRICT, n, BoundType::STRICT).sample());
				return false;
			}
			this->setLower(oldBound);
			this->setUpper(newBound);
		}
	}
	return false;
}

template<typename Number>
Sign RealAlgebraicNumberIR<Number>::sgn() const {
	if (this->interval.isZero()) return Sign::ZERO;
	if (this->interval.lower() < 0) return Sign::NEGATIVE;
	return Sign::POSITIVE;
}

template<typename Number>
Sign RealAlgebraicNumberIR<Number>::sgn(const UnivariatePolynomial<Number>& p) const {
	if (this->polynomial == p) {
		// this number is defined as a root of the given polynomial.
		return Sign::ZERO;
	}
	auto seq = this->polynomial.standardSturmSequence(this->polynomial.derivative() * p);
	int variations = UnivariatePolynomial<Number>::countRealRoots(seq, this->interval);
	assert((variations == -1) || (variations == 0) || (variations == 1));
	switch (variations) {
		case -1: return Sign::NEGATIVE;
		case 0: return Sign::ZERO;
		case 1: return Sign::POSITIVE;
	}
	LOGMSG_ERROR("carl.RANIR", "Unexpected number of variations, should be -1, 0, 1 but was " << variations);
	return Sign::ZERO;
}

}
