/**
 * @file AbstractRootFinder.tpp
 * @ingroup rootfinder
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#include "AbstractRootFinder.h"
#include "../logging.h"

#pragma once

namespace carl {
namespace rootfinder {

template<typename Number>
AbstractRootFinder<Number>::AbstractRootFinder(
		const UnivariatePolynomial<Number>& _polynomial,
		const Interval<Number>& _interval,
		bool tryTrivialSolver
	) :
		originalPolynomial(_polynomial),
		polynomial(_polynomial.squareFreePart()),
		//polynomial(polynomial),
		interval(_interval),
		finished(false)
{
#ifdef ROOTFINDER_CACHE
	if (this->inCache()) {
		CARL_LOG_TRACE("carl.core.rootfinder", "Hit cache: " << this->originalPolynomial);
		this->roots = cache[this->originalPolynomial];
		this->finished = true;
		return;
	}
#endif
	CARL_LOG_TRACE("carl.core.rootfinder", "Creating abstract rootfinder for " << polynomial << " with interval " << this->interval);
	if (this->polynomial.zeroIsRoot()) {
		CARL_LOG_DEBUG("carl.core.rootfinder", "Detected zero root in " << polynomial);
		this->addRoot(RealAlgebraicNumber<Number>(0));
	}
	if (this->polynomial.isZero()) {
		this->setFinished();
		return;
	}
	if (tryTrivialSolver && this->solveTrivial()) {
		CARL_LOG_TRACE("carl.core.rootfinder", "Polynomial was solved trivially.");
		this->setFinished();
		return;
	}
	if ((this->interval.lowerBoundType() == BoundType::INFTY) || (this->interval.upperBoundType() == BoundType::INFTY)) {
		Number bound = this->polynomial.cauchyBound();

		if (this->interval.lowerBoundType() == BoundType::INFTY) {
			this->interval.setLowerBoundType(BoundType::STRICT);
			if (-bound < this->interval.upper()) {
				this->interval.setLower(-bound);
			} else {
				this->interval.setLower(this->interval.upper());
			}
		}
		if (this->interval.upperBoundType() == BoundType::INFTY) {
			this->interval.setUpperBoundType(BoundType::STRICT);
			if (this->interval.lower() < bound) {
				this->interval.setUpper(bound);
			} else {
				this->interval.setUpper(this->interval.lower());
			}
		}
	}
	
	if (this->interval.lowerBoundType() == BoundType::WEAK) {
		this->interval.setLowerBoundType(BoundType::STRICT);
		if (this->polynomial.isRoot(this->interval.lower())) {
			this->addRoot(RealAlgebraicNumber<Number>(this->interval.lower()));
		}
	}
	if (this->interval.upperBoundType() == BoundType::WEAK) {
		this->interval.setUpperBoundType(BoundType::STRICT);
		if (this->polynomial.isRoot(this->interval.upper())) {
			this->addRoot(RealAlgebraicNumber<Number>(this->interval.upper()));
		}
	}
}

template<typename Number>
std::list<RealAlgebraicNumber<Number>> AbstractRootFinder<Number>::getAllRoots() {
	if (! this->isFinished()) {
		this->findRoots();
		this->setFinished();
	}
	this->roots.sort(carl::less<RealAlgebraicNumber<Number>>());
	return this->roots;
}

template<typename Number>
void AbstractRootFinder<Number>::addRoot(const RealAlgebraicNumber<Number>& root, bool reducePolynomial) {
	if (root.isNumeric()) {
		if (reducePolynomial) {
			this->polynomial.eliminateRoot(root.value());
		}
	}
	this->roots.push_back(root);
}

template<typename Number>
void AbstractRootFinder<Number>::addRoot(const Interval<Number>& interval) {
	this->addRoot(RealAlgebraicNumber<Number>(this->polynomial, interval));
}

template<typename Number>
bool AbstractRootFinder<Number>::solveTrivial() {
	CARL_LOG_DEBUG("carl.core.rootfinder", "Trying to trivially solve polynomial " << polynomial);
	switch (this->polynomial.degree()) {
		case 0: break;
		case 1: {
			CARL_LOG_DEBUG("carl.core.rootfinder", "Trivially solving linear polynomial " << polynomial);
			Number a = this->polynomial.coefficients()[1], b = this->polynomial.coefficients()[0];
			assert(a != Number(0));
			this->addRoot(RealAlgebraicNumber<Number>(-b / a), false);
			break;
		}
		case 2: {
			CARL_LOG_DEBUG("carl.core.rootfinder", "Trivially solving quadratic polynomial " << polynomial);
			Number a = this->polynomial.coefficients()[2], b = this->polynomial.coefficients()[1], c = this->polynomial.coefficients()[0];
			assert(a != Number(0));
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
					this->addRoot(RealAlgebraicNumber<Number>(this->polynomial, (Number(-b) - r) / Number(2*a)), false);
					this->addRoot(RealAlgebraicNumber<Number>(this->polynomial, (Number(-b) + r) / Number(2*a)), false);
				}
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
