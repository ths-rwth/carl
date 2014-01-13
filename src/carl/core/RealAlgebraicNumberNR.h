/* 
 * File:   RealAlgebraicNumberNR.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 * 
 * This file should never be included directly but only via RealAlgebraicNumber.h
 */

#include "RealAlgebraicNumber.h"


#pragma once

namespace carl {

/**
 * RealAlgebraicNumberNR is a specialization of RealAlgebraicNumber.
 * RealAlgebraicNumberNR always represents an exact number.
 */
// TODO: EnableIf<is_fraction<Number>>
template<typename Number>
class RealAlgebraicNumberNR : public RealAlgebraicNumber<Number> {
public:

	/**
	 * Construct a real algebraic number from a numeric <code>n</code>.
	 * @param n
	 * @param isRoot true marks this real algebraic number to stem from a root computation
	 */
	RealAlgebraicNumberNR(const Number& n, bool isRoot = true)
		: RealAlgebraicNumber<Number>(isRoot, true, n)
	{
	}

	/**
	 * Copy constructor.
	 * @param n
	 */
	RealAlgebraicNumberNR(const RealAlgebraicNumberNR& n)
		: RealAlgebraicNumber<Number>(n.isRoot(), true, n.value())
	{
	}
	
	bool equal(const RealAlgebraicNumberNR<Number>* n) {
		return this->value() == n->value();
	}
	bool less(const RealAlgebraicNumberNR<Number>* n) {
		return this->value() < n->value();
	}
	
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumberNR<Num>& n);
};

template<typename Number>
std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumberNR<Number>* n) {
	return os << "RealAlgebraicNumberNR(" << n->value() << ")";
}

}