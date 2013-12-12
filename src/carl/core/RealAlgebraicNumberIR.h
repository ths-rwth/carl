/* 
 * File:   RealAlgebraicNumberIR.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * This file should never be included directly but only via RealAlgebraicNumber.h
 */

#pragma once

#include "UnivariatePolynomial.h"
#include "../interval/ExactInterval.h"
#include "../util/SFINAE.h"

namespace carl {


template<typename Number, typename isFraction = EnableIf<is_fraction<Number>>>
class RealAlgebraicNumberIR {

};

template<typename Number>
class RealAlgebraicNumberIR<Number, EnableIf<is_fraction<Number>>> : public RealAlgebraicNumber<Number> {

protected:
	/**
	 * pointer to the polynomial of this interval representation
	 */
	UnivariatePolynomial<Number> polynomial;
	
	/**
	 * isolating interval of this interval representation
	 */
	ExactInterval<Number> interval;
	
	/**
	 * Standard Sturm sequence of the polynomial and its derivative
	 */
	std::list<UnivariatePolynomial<Number>> sturmSequence;
	
	/**
	 * number of refinements executed to the isolating interval
	 */
	unsigned int refinementCount;
	
public:
	
	//////////////////////////
	// Con- and destructors //
	//////////////////////////

	/**
	 * Constructs a real algebraic number in interval representation (p, l, r) with a normalized interval w.r.t. normalizeInterval.
	 */
	RealAlgebraicNumberIR();
	
	/**
	 * Constructs a real algebraic number in interval and order representation (p, l, r, o) with a normalized interval w.r.t. normalizeInterval.
	 *
	 * <ul>
	 * <li>p can be changed within the constructor, but is later constant.<li>
	 * <li>i can be changed later by refine(), but is only normalized within the constructor in case not normalized before.</li>
	 * </ul>
	 *
	 * @param p polynomial having the real algebraic number as one of its roots
	 * @param i open interval ]l, r[ containing the real algebraic number (should be normalized)
	 * @param s standard Sturm sequence
	 * @param normalize if set to false, the interval will not be normalized in the constructor (default is true)
	 * @param isRoot true marks this real algebraic number to stem from a root computation
	 */
	RealAlgebraicNumberIR(
			const UnivariatePolynomial<Number>& p,
			const ExactInterval<Number>& i,
			const std::list<UnivariatePolynomial<Number>>& s = std::list<UnivariatePolynomial<Number>>(),
			const bool normalize = true,
			const bool isRoot = true );

	/**
	 * Destructor.
	 */
	virtual ~RealAlgebraicNumberIR() {
	}
	
	///////////////
	// Selectors //
	///////////////

	bool isZero() const {
		return this->isNumeric() ? this->value() == 0 : (this->interval.left() == 0 && this->interval.right() == 0);
	}

	/**
	 * Selects the polynomial having this real algebraic number as one of its roots.
	 * @return polynomial having the number as one of its roots
	 */
	const UnivariatePolynomial<Number>& getPolynomial() const {
		return this->polynomial;
	}

	/**
	 * Selects the open interval ]l, r[ containing the real algebraic number.
	 * @return open interval ]l, r[ containing the real algebraic number
	 */
	const ExactInterval<Number>& getInterval() const
	{
		return this->interval;
	}

	/**
	 * Returns a pre-computed standard Sturm sequence of the polynomial and its derivative.
	 * @return standard Sturm sequence of the polynomial and its derivative.
	 */
	const std::list<UnivariatePolynomial<Number>>& getSturmSequence() const
	{
		return this->sturmSequence;
	}

	/** Returns how often one of the refine methods was called before.
	 * @return number of refinement steps executed on this real algebraic number
	 */
	unsigned getRefinementCount() const {
		return this->refinementCount;
	}
	
	///////////////
	// Operators //
	///////////////

	/**
	 * This number gets all values of the other.
	 */
	const RealAlgebraicNumberIR& operator=(const RealAlgebraicNumberIR& obj);
	
	////////////////
	// Operations //
	////////////////

	/** Normalizes the interval of an real algebraic number to not contain zero, in case the number is non-zero.
	 */
	void normalizeInterval();

	/**
	 * Coarsen the given isolating interval as much as possible while staying in the given interval.
	 * If interval is no over-approximation of the current isolating interval, do nothing.
	 * @param interval
	 */
	void coarsen(const ExactInterval<Number>& interval);

};

template<typename Number>
using RANIR = class RealAlgebraicNumberIR<Number, EnableIf<is_fraction<Number>>>;

}

#include "RealAlgebraicNumberIR.tpp"