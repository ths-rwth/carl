/**
 * @file RealAlgebraicNumberNR.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "RealAlgebraicNumber.h"

namespace carl {

/**
 * RealAlgebraicNumberNR is a specialization of RealAlgebraicNumber.
 * RealAlgebraicNumberNR always represents an exact number.
 */
/// @todo Add `EnableIf<is_fraction<Number>>` such that gcc does not crash.
template<typename Number>
class RealAlgebraicNumberNR : public RealAlgebraicNumber<Number> {
private:
	std::weak_ptr<RealAlgebraicNumberNR> pThis;
	std::shared_ptr<RealAlgebraicNumberNR> thisPtr() const {
		return std::shared_ptr<RealAlgebraicNumberNR>(this->pThis);
	}

	/**
	 * Construct a real algebraic number from a numeric <code>n</code>.
	 * @param n
	 * @param isRoot true marks this real algebraic number to stem from a root computation
	 */
	RealAlgebraicNumberNR(const Number& n, bool isRoot = true)
		: RealAlgebraicNumber<Number>(isRoot, true, n)
	{
	}
public:
	static std::shared_ptr<RealAlgebraicNumberNR> create(const Number& n, bool isRoot = true) {
		auto res = std::shared_ptr<RealAlgebraicNumberNR>(new RealAlgebraicNumberNR(n, isRoot));
		res->pThis = res;
		return res;
	}

	virtual std::shared_ptr<RealAlgebraicNumber<Number>> clone() const {
		return RealAlgebraicNumberNR<Number>::create(this->value(), this->isRoot());
	}
	
	bool equal(const RealAlgebraicNumberNR<Number>* n) {
		return this->value() == n->value();
	}
	bool less(const RealAlgebraicNumberNR<Number>* n) {
		return this->value() < n->value();
	}
	
	template<typename Num>
	friend std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumberNR<Num>* n);
};

template<typename Number>
using RealAlgebraicNumberNRPtr = std::shared_ptr<RealAlgebraicNumberNR<Number>>;

template<typename Number>
std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumberNR<Number>* n) {
	if (n == nullptr) return os << "nullptr";
	return os << "(NR " << n->value() << ")";
}

}