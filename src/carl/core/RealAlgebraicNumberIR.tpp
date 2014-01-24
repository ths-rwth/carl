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
		interval(),
		sturmSequence(this->polynomial.standardSturmSequence()),
		refinementCount(0)
{
}

template<typename Number>
RealAlgebraicNumberIR<Number>::RealAlgebraicNumberIR(
		const UnivariatePolynomial<Number>& p,
		const ExactInterval<Number>& i,
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
	if (normalize) this->normalizeInterval();
	if (this->interval.contains(0)) {
		this->mIsNumeric = true;
	}
	if (this->polynomial.degree() <= 1) {
		Number a = this->polynomial.coefficients()[1];
		Number b = this->polynomial.coefficients()[0];
		this->mValue = (a == 0) ? b : (-b/a);
		this->mIsNumeric = true;
		this->interval.setLeft(ExactInterval<Number>(this->interval.left(), this->value(), BoundType::STRICT).sample());
		this->interval.setRight(ExactInterval<Number>(this->value(), this->interval.right(), BoundType::STRICT).sample());
	}
	assert(p.countRealRoots(i) == 1);
}

template<typename Number>
const RealAlgebraicNumberIR<Number>& RealAlgebraicNumberIR<Number>::operator=(const RealAlgebraicNumberIR<Number>& obj) {
	this->interval = obj.getInterval();
	this->polynomial = obj.getPolynomial();
	this->sturmSequence = obj.getSturmSequence();
	this->refinementCount = obj.getRefinementCount();
	this->mIsNumeric = obj.isNumeric();
	this->mIsRoot = obj.isRoot();
	this->mValue = obj.value();
	return *this;
}

template<typename Number>
RealAlgebraicNumberIRPtr<Number> RealAlgebraicNumberIR<Number>::add(RealAlgebraicNumberIRPtr<Number>& n) {
	if (this->isZero() || n->isZero()) return n;

	Variable va = this->getPolynomial().mainVar();
	Variable vb = n->getPolynomial().mainVar();
	Variable y = VariablePool::getInstance().getFreshVariable();

	MultivariatePolynomial<Number> tmp1(this->getPolynomial());
	tmp1 = tmp1.substitute(va, MultivariatePolynomial<Number>({Term<Number>(va), -Term<Number>(vb)}));
	MultivariatePolynomial<Number> tmp2(n->getPolynomial().replaceVariable(y));
	UnivariatePolynomial<Number> res(tmp1.toUnivariatePolynomial(y).resultant(tmp2.toUnivariatePolynomial(y)).toNumberCoefficients());
	
	UnivariatePolynomial<typename IntegralT<Number>::type> ptmp = res.switchVariable(va).toIntegerDomain().primitivePart();
	auto p = ptmp.template convert<Number>();
	
	auto seq = p.standardSturmSequence();

	ExactInterval<Number> i = this->getInterval() + n->getInterval();
	while (p.isRoot(i.left()) || p.isRoot(i.right()) ||
		UnivariatePolynomial<Number>::countRealRoots(seq, i) > 0) {
		this->refine();
		n->refine();
		i = this->getInterval() + n->getInterval();
	}
	return RealAlgebraicNumberIR<Number>::create(p, i, seq);
}

template<typename Number>
bool RealAlgebraicNumberIR<Number>::equal(RealAlgebraicNumberIRPtr<Number> n) {
	if (this == n.get()) return true;
	if (n.get() == nullptr) return false;
	if (this->isZero() && n->isZero()) return true;
	if (this->right() <= n->left()) return false;
	if (this->left() >= n->right()) return false;
	return this->add(n)->isZero();
}

template<typename Number>
std::pair<bool,bool> RealAlgebraicNumberIR<Number>::checkOrder(RealAlgebraicNumberIRPtr<Number> n) {
	if (this->isNumeric()) {
		if (n->isNumeric()) {
			return std::make_pair(true, this->value() < n->value());
		} else {
			n->refineAvoiding(this->value());
			if (this->value() <= n->left()) {
				this->setRight(n->right());
				return std::make_pair(true, true);
			} else {
				assert(this->value() >= n->right());
				this->setRight(n->right());
				return std::make_pair(true, false);
			}
		}
	}
	if (n->isNumeric()) {
		this->refineAvoiding(n->value());
		if (n->value() <= this->left()) {
			n->setRight(this->left());
			return std::make_pair(true, false);
		} else {
			assert(n->value() >= this->right());
			n->setLeft(this->right());
			return std::make_pair(true, true);
		}
	}
	
	if (this->right() <= n->left()) {
		return std::make_pair(true, true);
	}
	if (n->right() <= this->left()) {
		return std::make_pair(true, false);
	}
	
	return std::make_pair(false, false);
}

template<typename Number>
std::pair<bool,bool> RealAlgebraicNumberIR<Number>::intervalContained(RealAlgebraicNumberIRPtr<Number> n, bool twisted) {
	if (this->getInterval().contains(n->getInterval())) {
		if (this->getPolynomial().isRoot(n->left())) {
			this->mValue = n->left();
			this->mIsNumeric = true;
			n->refineAvoiding(this->value());
			this->setRight(n->left());
			return std::make_pair(true, !twisted);
		} else if (this->getPolynomial().isRoot(n->right())) {
			this->mValue = n->right();
			this->mIsNumeric = true;
			n->refineAvoiding(this->value());
			this->setLeft(n->right());
			return std::make_pair(true, twisted);
		} else if (this->getPolynomial().countRealRoots(n->getInterval()) == 0) {
			if (this->left() != n->left() && this->getPolynomial().countRealRoots(ExactInterval<Number>(this->left(), n->left(), BoundType::STRICT)) > 0) {
				this->setRight(n->left());
				return std::make_pair(true, !twisted);
			} else if (this->right() != n->right() && this->getPolynomial().countRealRoots(ExactInterval<Number>(this->right(), n->right(), BoundType::STRICT)) > 0) {
				this->setLeft(n->right());
				return std::make_pair(true, twisted);
			}
			assert(this->getInterval() == n->getInterval());
		} else {
			this->setLeft(n->left());
			this->setRight(n->right());
		}
	}
	return std::make_pair(false, false);
}

template<typename Number>
bool RealAlgebraicNumberIR<Number>::checkIntersection(RealAlgebraicNumberIRPtr<Number> n, const ExactInterval<Number> i) {
	// Proceed only if this.left < n2.left and this.right < n2.right
	if ((this->left() < n->right()) && (this->left() < n->right())) {
		assert( i.left() == n->left() && i.right() == this->right());
		if (this->getPolynomial().isRoot(i.left())) {
			// If i.left is root of n1.polynomial: convert n1 to NR
			this->mValue = i.left();
			this->mIsNumeric = true;
			n->refineAvoiding(this->value());
			this->setRight(n->left());
			return true;
		}
		if (this->getPolynomial().countRealRoots(i) == 0) {
			// i contains no roots of n1.polynomial
			this->refineAvoiding(i.left());
			return true;
		}
		if (n->getPolynomial().isRoot(i.right())) {
			// If i.right is root of n2.polynomial: convert n2 to NR
			n->mValue = i.right();
			n->mIsNumeric = true;
			this->refineAvoiding(n->value());
			n->setLeft(this->right());
			return true;
		}
		if (n->getPolynomial().countRealRoots(i) == 0) {
			// i contains no roots of n2.polynomial
			n->refineAvoiding(i.right());
			return true;
		}
		this->setLeft(i.left());
		n->setRight(i.right());
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

		ExactInterval<Number> intersection = this->getInterval().intersect(n->getInterval());
		
		// invariant: intersection is nonempty and one bound belongs to an interval
		assert( !intersection.empty() );
		
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
	if (this->interval.left() == 0 && this->interval.right() == 0) return; // already normalized
	assert( this->polynomial.countRealRoots(this->interval) != 0); // the interval should be isolating for this number
	// shift the right border below 0 or set the zero interval
	if (this->interval.contains(0) && this->polynomial.sgn(0) == Sign::ZERO) {
		this->interval.set(0,0);
		this->mValue = 0;
		this->mIsNumeric = true;
	} else if (this->interval.meets(0)) {
		// one of the bounds might be 0
		// the separation is computed following [p.329,Algorithmic Algebra ISBN 3-540-94090-1]
		Number sep = (Number)1 / (1 + this->polynomial.maximumNorm());
		assert(this->polynomial.sgn(sep) != Sign::ZERO);
		if ((this->interval.right() == 0) || 
			(this->interval.left() < -sep && this->polynomial.countRealRoots(ExactInterval<Number>(this->interval.left(), -sep, BoundType::STRICT)) > 0)) {
			this->interval.setRight(-sep);
		} else {
			this->interval.setLeft(sep);
		}
	}
	assert( ( !this->interval.meets(0) && this->polynomial.countRealRoots(this->interval) == 1 ) || (this->interval.left() == 0 && this->interval.right() == 0)); // otherwise, the interval is not suitable for this real algebraic number
}

template<typename Number>
void RealAlgebraicNumberIR<Number>::coarsen(const ExactInterval<Number>& i)
{
	if (i.left() < this->interval.left()) { // coarsen the left bound
		Number l = this->interval.left();
		this->interval.setLeft(i.left());
		while (this->polynomial.sgn(this->interval.left()) == Sign::ZERO || countRealRoots(this->sturmSequence, this->interval) != 1) {
			this->interval.setLeft(ExactInterval<Number>(this->interval.left(), l).sampleFast());
		}
	}
	if (i.right() > this->interval.right()) { // coarsen the right bound
		Number r = this->interval.right();
		this->interval.setRight( interval.right() );
		while (this->polynomial->sgn(this->interval.right()) == Sign::ZERO || countRealRoots(this->sturmSequence, this->interval) != 1) {
			this->interval.setRight(ExactInterval<Number>(r, this->interval.right()).sampleFast());
		}
	}
}

template<typename Number>
void RealAlgebraicNumberIR<Number>::refine(RealAlgebraicNumberSettings::RefinementStrategy strategy) {
	if (this->isNumeric()) {
		// This RANIR already has a numeric value
		assert(this->left() != this->value());
		assert(this->right() != this->value());
		this->setLeft(ExactInterval<Number>(this->left(), this->value(), BoundType::STRICT).sample());
		this->setRight(ExactInterval<Number>(this->value(), this->right(), BoundType::STRICT).sample());
		return;
	}
	
	Number m;
	switch (strategy) {
		case RealAlgebraicNumberSettings::RefinementStrategy::GENERIC:
			m = this->getInterval().midpoint();
			break;
		case RealAlgebraicNumberSettings::RefinementStrategy::BINARYSAMPLE:
			m = this->getInterval().sample();
			break;
		case RealAlgebraicNumberSettings::RefinementStrategy::BINARYNEWTON:
			m = this->getInterval().sample();
			break;
	}
	
	if (this->getPolynomial().isRoot(m)) {
		this->setLeft(ExactInterval<Number>(this->left(), this->value(), BoundType::STRICT).sample());
		this->setRight(ExactInterval<Number>(this->value(), this->right(), BoundType::STRICT).sample());
		this->mValue = m;
		this->mIsNumeric = true;
	} else {
		if (this->getPolynomial().countRealRoots(ExactInterval<Number>(this->left(), m, BoundType::STRICT)) > 0) {
			this->setRight(m);
		} else {
			this->setLeft(m);
		}
	}
	
	this->refinementCount++;
	assert(this->left() < this->right());
}

template<typename Number>
bool RealAlgebraicNumberIR<Number>::refineAvoiding(const Number& n) {
	if (this->isNumeric()) {
		if (!this->getInterval().meets(n)) return false;
		if (this->value() < n) {
			this->setRight(ExactInterval<Number>(this->value(), n, BoundType::STRICT).sample());
		} else if (this->value() > n) {
			this->setLeft(ExactInterval<Number>(n, this->value(), BoundType::STRICT).sample());
		} else return true;
		assert(this->left() < this->right());
		return false;
	}
	
	if (this->getInterval().contains(n)) {
		if (this->getPolynomial().isRoot(n)) {
			this->mValue = n;
			this->mIsNumeric = true;
			return true;
		}
		if (this->getPolynomial().countRealRoots(ExactInterval<Number>(this->left(), n, BoundType::STRICT)) > 0) {
			this->setRight(n);
		} else {
			this->setLeft(n);
		}
		this->refinementCount++;
	} else if (this->left() != n && this->right() != n) {
		return false;
	}
	
	bool isLeft = this->left() == n;
	
	Number newBound = this->getInterval().sample();
	
	if (this->getPolynomial().isRoot(newBound)) {
		this->mValue = newBound;
		this->mIsNumeric = true;
		if (isLeft) {
			this->setLeft(ExactInterval<Number>(n, newBound, BoundType::STRICT).sample());
		} else {
			this->setRight(ExactInterval<Number>(newBound, n, BoundType::STRICT).sample());
		}
	}
	
	if (isLeft) {
		this->setLeft(newBound);
	} else {
		this->setRight(newBound);
	}
	
	while (this->getPolynomial().countRealRoots(this->getInterval()) == 0) {
		if (isLeft) {
			Number oldBound = this->left();
			Number newBound = ExactInterval<Number>(n, oldBound, BoundType::STRICT).sample();
			if (this->getPolynomial().isRoot(newBound)) {
				this->mValue = newBound;
				this->mIsNumeric = true;
				this->setLeft(ExactInterval<Number>(n, newBound, BoundType::STRICT).sample());
				return false;
			}
			this->setRight(oldBound);
			this->setLeft(newBound);
		} else {
			Number oldBound = this->right();
			Number newBound = ExactInterval<Number>(oldBound, n, BoundType::STRICT).sample();
			if (this->getPolynomial().isRoot(newBound)) {
				this->mValue = newBound;
				this->mIsNumeric = true;
				this->setRight(ExactInterval<Number>(newBound, n, BoundType::STRICT).sample());
				return false;
			}
			this->setLeft(oldBound);
			this->setRight(newBound);
		}
	}
	return false;
}

template<typename Number>
Sign RealAlgebraicNumberIR<Number>::sgn() const {
	if (this->interval.isZero()) return Sign::ZERO;
	if (this->interval.left() < 0) return Sign::NEGATIVE;
	return Sign::POSITIVE;
}

template<typename Number>
Sign RealAlgebraicNumberIR<Number>::sgn(const UnivariatePolynomial<Number>& p) const {
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