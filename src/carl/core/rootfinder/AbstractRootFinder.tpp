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
		const UnivariatePolynomial<Number>& polynomial,
		const ExactInterval<Number>& interval,
		bool tryTrivialSolver
	) :
		originalPolynomial(polynomial),
		polynomial(polynomial.squareFreePart()),
		//polynomial(polynomial),
		interval(interval),
		finished(false)
{
	LOGMSG_TRACE("carl.core.rootfinder", "Creating abstract rootfinder for " << polynomial);
	if (this->polynomial.zeroIsRoot()) {
		this->polynomial.eliminateZeroRoots();
		this->addRoot(RealAlgebraicNumberNR<Number>::create(0));
	}
	if (tryTrivialSolver && this->solveTrivial()) {
		LOGMSG_TRACE("carl.core.rootfinder", "Polynomial was solved trivially.");
		this->finished = true;
	}
	if (this->interval.leftType() == BoundType::INFTY) {
		this->interval.setLeft(-this->polynomial.cauchyBound());
		this->interval.setLeftType(BoundType::STRICT);
	}
	if (this->interval.rightType() == BoundType::INFTY) {
		this->interval.setRight(this->polynomial.cauchyBound());
		this->interval.setRightType(BoundType::STRICT);
	}
}

template<typename Number>
std::list<RealAlgebraicNumberPtr<Number>> AbstractRootFinder<Number>::getAllRoots() {
	if (! this->isFinished()) {
		this->findRoots();
		this->setFinished();
	}
	return this->roots;
}

template<typename Number>
void AbstractRootFinder<Number>::addRoot(RealAlgebraicNumberPtr<Number> root, bool reducePolynomial) {
	if (root->isNumeric()) {
		if (reducePolynomial) {
			this->polynomial.eliminateRoot(root->value());
		}
	} else {
		RealAlgebraicNumberIRPtr<Number> r = std::static_pointer_cast<RealAlgebraicNumberIR<Number>>(root);
		if (r->getInterval().diameter() == 0) {
			root = RealAlgebraicNumberNR<Number>::create(r->getInterval().left());
		}
	}
	this->roots.push_back(root);
}

template<typename Number>
void AbstractRootFinder<Number>::addRoot(const ExactInterval<Number>& interval) {
	this->addRoot(RealAlgebraicNumberIR<Number>::create(this->polynomial, interval));
}

template<typename Number>
bool AbstractRootFinder<Number>::solveTrivial() {
		switch (this->polynomial.degree()) {
			case 0: {
				if (this->polynomial.coefficients()[0] == 0) {
					this->addRoot(RealAlgebraicNumberNR<Number>::create(0), false);
				}
				break;
			}
			case 1: {
				Number a = polynomial.coefficients()[1], b = polynomial.coefficients()[0];
				this->addRoot(RealAlgebraicNumberNR<Number>::create(-b / a), false);
				break;
			}
			case 2: {
				Number a = polynomial.coefficients()[2], b = polynomial.coefficients()[1], c = polynomial.coefficients()[0];
				/* Use this formulation of p-q-formula:
				 * x = ( -b +- \sqrt{ b*b - 4*a*c } ) / (2*a)
				 */
				Number rad = b*b - 4*a*c;
				if (rad == 0) {
					this->addRoot(RealAlgebraicNumberNR<Number>::create(-b / (2*a)), false);
				} else if (rad > 0) {
					std::pair<Number, Number> res = carl::sqrt_fast(rad);
					if (res.first == res.second) {
						// Root could be calculated exactly
						this->addRoot(RealAlgebraicNumberNR<Number>::create((-b - res.first) / (2*a)), false);
						this->addRoot(RealAlgebraicNumberNR<Number>::create((-b + res.second) / (2*a)), false);
					} else {
						// Root is within interval (res.first, res.second)
						ExactInterval<Number> r(res.first, res.second, BoundType::STRICT);
						this->addRoot(RealAlgebraicNumberIR<Number>::create(this->polynomial, (-b - r) / (2*a)), false);
						this->addRoot(RealAlgebraicNumberIR<Number>::create(this->polynomial, (-b + r) / (2*a)), false);
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
