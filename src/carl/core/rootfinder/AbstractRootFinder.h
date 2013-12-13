/* 
 * File:   AbstractRootFinder.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../../interval/ExactInterval.h"
#include "../RealAlgebraicNumber.h"
#include "../RealAlgebraicNumberIR.h"
#include "../RealAlgebraicNumberNR.h"
#include "../UnivariatePolynomial.h"

namespace carl {
namespace core {

template<typename Number>
class AbstractRootFinder {
protected:
	/**
	 * Original polynomial to solve.
	 * Stored for informational purposes.
	 */
	UnivariatePolynomial<Number> originalPolynomial;
	/**
	 * Polynomial to solve.
	 * This polynomial may be changed during the solving process.
	 */
	UnivariatePolynomial<Number> polynomial;
	/**
	 * Interval to be searched.
	 */
	ExactInterval<Number> interval;
	/**
	 * Roots that have been found.
	 * If the instantiated root finder works in an incremental manner, this list may not contain all roots, if it is called before the root finder has indicated that it is finished.
	 */
	std::list<RealAlgebraicNumber<Number>*> roots;
	/**
	 * Flag that indicates if the search has finished.
     */
	bool finished;

public:
	/**
	 * Constructor of an AbstractRootFinder.
	 * This constructor already does some preprocessing on the given polynomial.
	 * At first, the polynomial is tested for a root at zero. If there is such a root, it is added to the root list and removed from the polynomial.
	 * If tryTrivialSolver is true, it will then try to solve the polynomial directly using solveTrivial().
	 * If the interval is unbounded (in either direction), the cauchyBound() method is used to the polynomial to construct a bounded interval.
     * @param polynomial Polynomial to solve.
     * @param interval Interval bounding the range of the roots we are looking for.
	 * @param tryTrivialSolver Flag indicating if the trivialSolve() method should be used.
     */
	AbstractRootFinder(
		UnivariatePolynomial<Number>& polynomial, 
		const ExactInterval<Number>& interval = ExactInterval<Number>(),
		bool tryTrivialSolver = true
	);
	
	virtual ~AbstractRootFinder() {
	}
	
	/**
	 * Returns the list of roots.
	 * If called before computation is finished, this list will not contain all roots and will be in arbitrary order.
	 * Once all roots have been isolated, this list will contain all roots in increasing order.
	 * @return List of isolated roots.
	 */
	inline std::list<RealAlgebraicNumber<Number>*> getRoots() const {
	   return this->roots;
	}
	
	/**
	 * Convenience method to obtain all roots at once in a sorted order.
	 * @returns List of roots.
	 */
	std::list<RealAlgebraicNumber<Number>*> getAllRoots();
	
protected:
	/**
	 * Adds a new root to the internal root list.
	 * If the root is in numeric representation, it will eliminate the root from the polynomial unless reducePolynomial is false.
	 * @param root Pointer to new root.
	 * @param reducePolynomial Indicates if the polynomial should be reduced.
	 */
	virtual void addRoot(RealAlgebraicNumber<Number>* root, bool reducePolynomial = true);
	
	/**
	 * Adds a new root to the internal root list from an interval.
     * @param interval Isolating interval.
     */
	virtual void addRoot(const ExactInterval<Number>& interval);
	
	/**
	 * Informational method for subclasses specifying the maximum degree of the polynomial that solveTrivial() can handle.
     * @return Maximum degree that solveTrivial() can handle.
     */
	unsigned int solveTrivialMaxDegree() const {
		return 2;
	}
	
	/**
	 * Find roots for polynomials of degree at most two and adds them to roots.
	 * The resulting roots will always be in sorted order.
	 * @returns true, if polynomial has degree at most two.
	 */
	bool solveTrivial();
	
	/**
	 * Find all roots of the polynomial.
	 * This method must be implemented by each subclass.
	 * All subclasses can safely assume the following about the given polynomial:
	 * - It has no root at zero.
	 * - It has degree at least three.
	 */
	virtual void findRoots() = 0;
	
	/**
	 * Indicates if the root finding process has finished.
	 */
	bool isFinished() const {
		return this->finished;
	}
	
	/**
	 * Set status to finished.
	 * Sorts roots that have been found.
     */
	void setFinished() {
		if (! this->isFinished()) {
			this->finished = true;
			this->roots.sort();
		}
	}
};

}
}

#include "AbstractRootFinder.tpp"