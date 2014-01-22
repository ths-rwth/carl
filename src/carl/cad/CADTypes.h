/* 
 * @file CADTypes.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <memory>

#include "../core/MultivariatePolynomial.h"
#include "../core/UnivariatePolynomial.h"

namespace carl {
namespace cad {

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
public:
	~PolynomialOwner() {
		for (auto p: this->ownedPolynomials) {
			delete p;
		}
	}
	
	void takeOwnership(const UPolynomial<Coeff>* p) {
		this->ownedPolynomials.push_back(p);
	}
};

}
}
