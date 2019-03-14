/**
 * @file CADTypes.h
 * @ingroup cad
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <memory>

#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/UnivariatePolynomial.h>
#include <carl/core/logging.h>

namespace carl {
namespace cad {
using carl::operator<<;

enum Answer { True = 0, False = 1, Unknown = 2 };

template<typename Coeff>
using MPolynomial = carl::MultivariatePolynomial<Coeff>;

template<typename Coeff>
using UPolynomial = carl::UnivariatePolynomial<MPolynomial<Coeff>>;

/**
 * This class manages the ownership of pointers to UnivariatePolynomial objects.
 * It is intended to be subclassed, for example by the CAD class.
 * 
 * The ownership of a pointer can be delegated by calling takeOwnership().
 * A pointer that has been delegated must not be deleted manually.
 * This will automatically be done when this class is destroyed.
 */
template<typename Coeff>
class PolynomialOwner {
private:
	/**
	 * This list contains all polynomials that are owned by this EliminationSet.
	 */
	std::list<const UPolynomial<Coeff>*> ownedPolynomials;

	/**
	 * The parent PolynomialOwner.
	 * If set, all polynomials are delegated to the parent.
	 */
	PolynomialOwner<Coeff>* parentOwner;
public:
	/**
	 * Constructs a PolynomialOwner.
     */
	PolynomialOwner() {
		this->parentOwner = nullptr;
	}
	/**
	 * Constructs a delegating PolynomialOwner.
	 * All polynomials are delegated to the given parent.
     * @param parent Parent PolynomialOwner.
     */
	PolynomialOwner(PolynomialOwner<Coeff>* parent) {
		this->parentOwner = parent;
	}

	/**
	 * Destroys all owned polynomials.
     */
	virtual ~PolynomialOwner() {
		if (this->ownedPolynomials.size() > 0) {
			CARL_LOG_DEBUG("carl.cad", "Deleting " << this->ownedPolynomials.size() << " polynomials.");
		}
		for (auto p: this->ownedPolynomials) {
			delete p;
		}
	}
	
	/**
	 * Takes ownership of the given polynomial and returns it.
     * @param p Polynomial.
     * @return p.
     */
	const UPolynomial<Coeff>* take(const UPolynomial<Coeff>* p) {
		if (this->parentOwner != nullptr) return this->parentOwner->take(p);
		assert(std::find(ownedPolynomials.begin(), ownedPolynomials.end(), p) == ownedPolynomials.end());
		this->ownedPolynomials.push_back(p);
		return p;
	}
	/**
	 * Takes ownership of the given polynomial and returns it.
     * @param p Polynomial.
     * @return p.
     */
	UPolynomial<Coeff>* take(UPolynomial<Coeff>* p) {
		if (this->parentOwner != nullptr) return this->parentOwner->take(p);
		assert(std::find(ownedPolynomials.begin(), ownedPolynomials.end(), p) == ownedPolynomials.end());
		this->ownedPolynomials.push_back(p);
		return p;
	}
};

}
}
