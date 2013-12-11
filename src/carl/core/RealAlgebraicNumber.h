/* 
 * File:   RealAlgebraicNumber.h
 * Author: Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../util/SFINAE.h"
#include "../numbers/numbers.h"
#include "../core/Sign.h"
#include "UnivariatePolynomial.h"

namespace carl {

/**
 * This class is the base for all representations of real algebraic numbers and provides crucial operations such as arithmetic, ordering or sign determination on them.
 */
template<typename Number, EnableIf<is_fraction<Number>> = dummy>
class RealAlgebraicNumber {
protected:
	////////////////
	// Attributes //
	////////////////

	/**
	 * flag indicating whether this number represents a root of a polynomial or an intermediate point
	 */
	bool mIsRoot;
	/**
	 * flag indicating whether this number is representable by a numeric
	 */
	bool mIsNumeric;
	/**
	 * the exact numeric value of this number if available, otherwise mIsNumeric is false and mValue 0
	 */
	Number mValue;

public:
	//////////////////////////
	// Con- and destructors //
	//////////////////////////

	/**
	 * Constructs a real algebraic number with a specified marking as to whether this number stems from a real root computation or not.
	 * @param isRoot true marks this real algebraic number to stem from a root computation
	 * @param isNumeric true marks this real algebraic number to be representable as an exact numeric (standard is false)
	 * @param value the exact numeric value of this number if available, otherwise mIsNumeric is false and mValue 0
	 */
	RealAlgebraicNumber(bool isRoot, bool isNumeric = false, const Number& value = 0) :
			mIsRoot(isRoot),
			mIsNumeric(isNumeric),
			mValue(value)
	{
	}
	/**
	 * Destructor.
	 */
	~RealAlgebraicNumber() {
	}

	///////////////
	// Selectors //
	///////////////
	/**
	 * @return the flag marking whether the real algebraic number stems from a root computation or not
	 */
	bool isRoot() const {
		return mIsRoot;
	}

	/**
	 * Set the flag marking whether the real algebraic number stems from a root computation or not.
	 * @param isRoot
	 */
	void setIsRoot(bool isRoot) {
		mIsRoot = isRoot;
	}

	/**
	 * Returns true if the number is exactly zero.
	 * @return true if an exact numeric representation was found during the refinements, false otherwise.
	 */
	virtual bool isZero() const {
		return this->value() == 0;
	}

	/**
	 * Returns true if an exact numeric representation was found during the refinements.
	 * @return true if an exact numeric representation was found during the refinements, false otherwise.
	 */
	bool isNumeric() const {
		return mIsNumeric;
	}

	/** Gives an exact numeric representation of this real algebraic number which could have been found during the refinement steps.
	 * The method returns 0 if the value was never set during refinement.
	 * @return an exact numeric representation of this real algebraic number which could have been found during the refinement steps
	 */
	const Number value() const {
		return mValue;
	}
	
	///////////////
	// Operators //
	///////////////

	// assignment operators

	/**
	 * This real algebraic number gets all values of the other.
	 * @param o other real algebraic number
	 */
	virtual const RealAlgebraicNumber<Number>& operator=(const RealAlgebraicNumber<Number>& o) {
		mIsRoot = o.mIsRoot;
		return *this;
	}

	////////////////
	// Operations //
	////////////////

	/**
	 * Returns sign (GiNaC::ZERO_SIGN, GiNaC::POSITIVE_SIGN, GiNaC::NEGATIVE_SIGN) of this real algebraic number.
	 * @return sign of this real algebraic number.
	 */
	virtual Sign sgn() const {
		if (this->value() == 0) return Sign::ZERO;
		else if (this->value() > 0) return Sign::POSITIVE;
		else return Sign::NEGATIVE;
	}

	/**
	 * Returns sign (GiNaC::ZERO_SIGN, GiNaC::POSITIVE_SIGN, GiNaC::NEGATIVE_SIGN) of the specified univariate polynomial at this real algebraic number.
	 * @param p rational univariate polynomial
	 * @return sign of the univariate polynomial at this real algebraic number.
	 */
	virtual Sign sgn(const UnivariatePolynomial<Number>& p) const {
		Number x = p.evaluate(this->value());
		if (x == 0) return Sign::ZERO;
		else if (x > 0) return Sign::POSITIVE;
		else return Sign::NEGATIVE;
	}

	/** Computes a numeric value for this real algebraic number approximating it.
	 * @complexity constant
	 * @return a numeric value for this real algebraic number approximating it
	 */
	virtual const Number approximateValue() const {
		return this->value();
	}
	
	friend std::ostream& operator<<(std::ostream& os, const RealAlgebraicNumber<Number>& g) {
		return os << "RealAlgebraicNumber(" << g.value() << ", " << g.isNumeric() << ", " << g.isRoot() << ")";
	}
};

}
