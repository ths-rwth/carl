/* 
 * File:   RealAlgebraicNumberIR.tpp
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../util/SFINAE.h"
#include "UnivariatePolynomial.h"
#include "RealAlgebraicNumberIR.h"

namespace carl {
namespace core {
template<typename Number>
RANIR<Number>::RealAlgebraicNumberIR() :
		RealAlgebraicNumber<Number>(true, true, 0),
		polynomial(),
		interval(),
		sturmSequence(UnivariatePolynomial<Number>::standardSturmSequence( this->polynomial, this->polynomial.derivative())),
		refinementCount( 0 )
{
}

template<typename Number>
RANIR<Number>::RealAlgebraicNumberIR(
		const UnivariatePolynomial<Number>& p,
		const ExactInterval<Number>& i,
		const std::list<UnivariatePolynomial<Number>>& seq,
		const bool normalize,
		const bool isRoot ) :
        RealAlgebraicNumber<Number>(isRoot, false, 0),
        polynomial(p.squareFreePart().template convert<Number>()),
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
}

template<typename Number>
const RANIR<Number>& RANIR<Number>::operator=(const RANIR<Number>& obj) {
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
void RANIR<Number>::normalizeInterval() {
	if (this->interval.left() == 0 && this->interval.right() == 0) return; // already normalized
	assert( this->polynomial.countRealRoots(this->interval) != 0); // the interval should be isolating for this number
	// shift the right border below 0 or set the zero interval
	if (this->interval.contains(0) && sgn(this->polynomial.evaluate(0)) == Sign::ZERO) {
		this->interval.set(0,0);
		this->mValue = 0;
		this->mIsNumeric = true;
	} else if (this->interval.meets(0)) {
		// one of the bounds might be 0
		// the separation is computed following [p.329,Algorithmic Algebra ISBN 3-540-94090-1]
		Number sep = (Number)1 / (1 + this->polynomial.maximumNorm());
		assert(sgn(this->polynomial.evaluate(sep)) != Sign::ZERO);
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
void RANIR<Number>::coarsen(const ExactInterval<Number>& i)
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

}
}