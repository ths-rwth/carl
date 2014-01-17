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

/// @todo Add `EnableIf<is_fraction<Number>>` such that gcc does not crash.
template<typename Number>
class RealAlgebraicNumberIR : public RealAlgebraicNumber<Number> {

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

	RealAlgebraicNumberIR(const Variable& var);
	
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
	
	const Number& left() const {
		return this->getInterval().left();
	}
	const Number& right() const {
		return this->getInterval().right();
	}
	
	void setLeft(const Number& n) {
		return this->interval.setLeft(n);
	}
	void setRight(const Number& n) {
		return this->interval.setRight(n);
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
	
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const carl::RealAlgebraicNumberIR<Num>* n);

	virtual bool containedIn(const ExactInterval<Number>& i) const {
		return i.contains(this->getInterval());
	}

	bool equal(const RealAlgebraicNumberIR* n) const;
	
private:
	// Helper functions for lessWhileUnequal
	
	/**
	 * Checks if *this < *n.
	 * If the method could not decide the order, it returns (false, false)
	 * Otherwise, it returns (true, X), X being the value of *this < *n
     * @param n Another RealAlgebraicNumberIR
     * @return 
     */
	std::pair<bool,bool> checkOrder(RealAlgebraicNumberIR* n);
	std::pair<bool,bool> intervalContained(RealAlgebraicNumberIR* n, bool twisted);
	bool checkIntersection(RealAlgebraicNumberIR* n, const ExactInterval<Number> i);
	

public:	
	bool lessWhileUnequal(RealAlgebraicNumberIR* n);
	
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
	
	
	/** Refines the interval i of this real algebraic number yielding the interval j such that <code>2*(j.Right()-j.Left()) &lt;= i.Right()-i.Left()</code>. This is cutting the interval in the middle and choosing the half where the root lays in.
	 * @param strategy strategy selection according to RealAlgebraicNumberFactory::searchRealRootsStrategy
	 * @rcomplexity constant
	 * @scomplexity constant
	 */
	void refine(RealAlgebraicNumberSettings::RefinementStrategy strategy = RealAlgebraicNumberSettings::RefinementStrategy::DEFAULT);

	/** Refines the interval i of this real algebraic number yielding the interval j such that <code>(j.Right()-j.Left()) &lt;= eps</code>.
	 * @param eps
	 */
	inline void refine(const Number& eps) {
		while (this->getInterval().diameter() > eps)
			this->refine();
	}

	/** Refines the interval i of this real algebraic number yielding the interval j such that !j.meets(n). If true is returned, n is the exact numeric representation of this root. Otherwise not.
	 * @param n
	 * @rcomplexity constant
	 * @scomplexity constant
	 * @return true, if n is the exact numeric representation of this root, otherwise false
	 */
	bool refineAvoiding(const Number& n);

	Sign sgn() const;

	Sign sgn(const UnivariatePolynomial<Number>& p) const;

};

template<typename Number>
std::ostream& operator<<(std::ostream& os, const carl::RealAlgebraicNumberIR<Number>* n) {
	return os << "(IR " << n->getInterval() << ", " << n->getPolynomial() << ")";
}

}

#include "RealAlgebraicNumberIR.tpp"