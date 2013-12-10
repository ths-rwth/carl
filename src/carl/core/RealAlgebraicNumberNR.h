/* 
 * File:   RealAlgebraicNumberNR.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "RealAlgebraicNumber.h"

namespace carl {
namespace core {

/**
 * RealAlgebraicNumberNR is a specialization of RealAlgebraicNumber.
 * RealAlgebraicNumberNR always represents an exact number.
 */
template<typename Number, EnableIf<is_fraction<Number>> = dummy>
class RealAlgebraicNumberNR : RealAlgebraicNumber<Number> {
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
	
	friend std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumberNR<Number>& g);
};

template<typename Number>
std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumberNR<Number>& g) {
	return os << "RealAlgebraicNumberNR(" << g.value() << ")";
}

}
}