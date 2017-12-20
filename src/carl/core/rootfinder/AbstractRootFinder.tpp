/**
 * @file AbstractRootFinder.tpp
 * @ingroup rootfinder
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#include "AbstractRootFinder.h"
#include "../logging.h"
#include "../polynomialfunctions/RootBounds.h"

#pragma once

namespace carl {
namespace rootfinder {

template<typename Number>
AbstractRootFinder<Number>::AbstractRootFinder(
		const UnivariatePolynomial<Number>& _polynomial,
		const Interval<Number>& _interval,
		bool tryTrivialSolver
	) :
		mOriginalPolynomial(_polynomial),
		mPolynomial(_polynomial.squareFreePart()),
		mInterval(_interval),
		mFinished(false)
{
#ifdef ROOTFIND1ER_CACHE
	if (restoreFromCache()) return;
#endif
	CARL_LOG_TRACE("carl.core.rootfinder", "Creating abstract rootfinder for " << mPolynomial << " with interval " << mInterval);
	if (mPolynomial.zeroIsRoot()) {
		CARL_LOG_DEBUG("carl.core.rootfinder", "Detected zero root in " << mPolynomial);
		addRoot(RealAlgebraicNumber<Number>(0));
	}
	if (mPolynomial.isZero()) {
		setFinished();
		return;
	}
	if (tryTrivialSolver && solveTrivial()) {
		CARL_LOG_TRACE("carl.core.rootfinder", "Polynomial was solved trivially.");
		setFinished();
		return;
	}
	if ((mInterval.lowerBoundType() == BoundType::INFTY) || (mInterval.upperBoundType() == BoundType::INFTY)) {
		CARL_LOG_TRACE("carl.core.rootfinder", "Generating artificial bound.");
		Number bound = cauchyBound(mPolynomial);

		if (mInterval.lowerBoundType() == BoundType::INFTY) {
			mInterval.setLowerBoundType(BoundType::STRICT);
			if (-bound < mInterval.upper()) {
				mInterval.setLower(-bound);
			} else {
				mInterval.setLower(mInterval.upper());
			}
		}
		if (mInterval.upperBoundType() == BoundType::INFTY) {
			mInterval.setUpperBoundType(BoundType::STRICT);
			if (mInterval.lower() < bound) {
				mInterval.setUpper(bound);
			} else {
				mInterval.setUpper(mInterval.lower());
			}
		}
	}
	
	if (mInterval.lowerBoundType() == BoundType::WEAK) {
		mInterval.setLowerBoundType(BoundType::STRICT);
		if (mPolynomial.isRoot(mInterval.lower())) {
			this->addRoot(RealAlgebraicNumber<Number>(mInterval.lower()));
		}
	}
	if (mInterval.upperBoundType() == BoundType::WEAK) {
		mInterval.setUpperBoundType(BoundType::STRICT);
		if (mPolynomial.isRoot(mInterval.upper())) {
			this->addRoot(RealAlgebraicNumber<Number>(mInterval.upper()));
		}
	}
	CARL_LOG_TRACE("carl.core.rootfinder", "Bounds: " << mInterval);
}

template<typename Number>
const std::vector<RealAlgebraicNumber<Number>>& AbstractRootFinder<Number>::getAllRoots() {
	if (!isFinished()) {
		this->findRoots();
		this->setFinished();
	}
	return mRoots;
}

template<typename Number>
void AbstractRootFinder<Number>::addRoot(const RealAlgebraicNumber<Number>& root, bool reducePolynomial) {
	if (reducePolynomial && root.isNumeric()) {
		CARL_LOG_DEBUG("carl.core.rootfinder", "Eliminating root from " << mPolynomial);
		mPolynomial.eliminateRoot(root.value());
		CARL_LOG_DEBUG("carl.core.rootfinder", "-> " << mPolynomial);
	}
	mRoots.push_back(root);
}

template<typename Number>
void AbstractRootFinder<Number>::addRoot(const Interval<Number>& interval) {
	CARL_LOG_DEBUG("carl.core.rootfinder", "Constructing RAN from " << mPolynomial << " and " << interval);
	this->addRoot(RealAlgebraicNumber<Number>(mPolynomial, interval));
}

template<typename Number>
bool AbstractRootFinder<Number>::solveTrivial() {
	CARL_LOG_DEBUG("carl.core.rootfinder", "Trying to trivially solve polynomial " << mPolynomial);
	switch (mPolynomial.degree()) {
		case 0: break;
		case 1: {
			CARL_LOG_DEBUG("carl.core.rootfinder", "Trivially solving linear polynomial " << mPolynomial);
			const auto& a = mPolynomial.coefficients()[1];
			const auto& b = mPolynomial.coefficients()[0];
			assert(!carl::isZero(a));
			this->addRoot(RealAlgebraicNumber<Number>(-b / a), false);
			break;
		}
		case 2: {
			CARL_LOG_DEBUG("carl.core.rootfinder", "Trivially solving quadratic polynomial " << mPolynomial);
			const auto& a = mPolynomial.coefficients()[2];
			const auto& b = mPolynomial.coefficients()[1];
			const auto& c = mPolynomial.coefficients()[0];
			assert(!carl::isZero(a));
			/* Use this formulation of p-q-formula:
			 * x = ( -b +- \sqrt{ b*b - 4*a*c } ) / (2*a)
			 */
			Number rad = b*b - 4*a*c;
			if (rad == 0) {
				this->addRoot(RealAlgebraicNumber<Number>(-b / (2*a)), false);
			} else if (rad > 0) {
				std::pair<Number, Number> res = carl::sqrt_fast(rad);
				if (res.first == res.second) {
					// Root could be calculated exactly
					this->addRoot(RealAlgebraicNumber<Number>((-b - res.first) / (2*a)), false);
					this->addRoot(RealAlgebraicNumber<Number>((-b + res.first) / (2*a)), false);
				} else {
					// Root is within interval (res.first, res.second)
					Interval<Number> r(res.first, BoundType::STRICT, res.second, BoundType::STRICT);
					this->addRoot(RealAlgebraicNumber<Number>(mPolynomial, (Number(-b) - r) / Number(2*a)), false);
					this->addRoot(RealAlgebraicNumber<Number>(mPolynomial, (Number(-b) + r) / Number(2*a)), false);
				}
			} else {
				// No root.
			}
			break;
		}
		default:
			return false;
	}
	return true;
}

}
}
