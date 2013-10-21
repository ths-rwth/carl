/**
 * @file UnivariatePolynomial.h 
 * @author Sebastian Junges
 */

#pragma once
#include <map>

#include "../numbers/numbers.h"
#include "Variable.h"
#include "VariableInformation.h"
#include "Polynomial.h"
#include "DivisionResult.h"


namespace carl
{

template<typename Coefficient>
class UnivariatePolynomial : public Polynomial
{
public:
	UnivariatePolynomial(Variable::Arg mainVar);
	UnivariatePolynomial(Variable::Arg mainVar, std::initializer_list<Coefficient> coefficients);
	UnivariatePolynomial(Variable::Arg mainVar, const std::map<unsigned, Coefficient>& coefficients);
	UnivariatePolynomial(Variable::Arg mainVar, const VariableInformation<true, Coefficient>& varinfoWithCoefficients);

	//Polynomial interface implementations.

	/**
	 * @see class Polynomial
	 * @return 
	 */
	virtual bool isUnivariateRepresented() const override
	{
		return true;
	}

	/**
	 * @see class Polynomial
	 * @return 
	 */
	virtual bool isMultivariateRepresented() const override
	{
		return false;
	}

	bool isZero() const
	{
		return mCoefficients.size() == 0;
	}
	
	bool isOne() const
	{
		return mCoefficients.size() == 1 && mCoefficients.back() == (Coefficient)1;
	}

	/**
	 * Checks whether the polynomial is constant with respect to the main variable.
	 * @return 
	 */
	bool isConstant() const
	{
		return mCoefficients.size() <= 1;
	}

	/**
	 * Get the maximal exponent of the main variable.
	 * @return 
	 */
	unsigned degree() const
	{
		// todo add assertion that this is valid.
		return mCoefficients.size() == 0 ? 0 : (unsigned)mCoefficients.size()-1;
	}

	const Coefficient& lcoeff() const
	{
		return mCoefficients.back();
	}

	/**
	 * 
     * @return copr
     */
	template<typename Integer>
	UnivariatePolynomial<Integer> coprimeCoefficients() const;
	
	UnivariatePolynomial derivative(unsigned nth = 1) const;

	UnivariatePolynomial reduce(const UnivariatePolynomial& divisor) const;
	
	DivisionResult<UnivariatePolynomial> divide(const UnivariatePolynomial& divisor) const;
	
	UnivariatePolynomial& mod(const Coefficient& modulus);
	UnivariatePolynomial mod(const Coefficient& modulus) const;
	static UnivariatePolynomial& mod(UnivariatePolynomial&, const Coefficient& modulus);
	static UnivariatePolynomial mod(const UnivariatePolynomial&, const Coefficient& modulus);
	static UnivariatePolynomial gcd(const UnivariatePolynomial& p, const UnivariatePolynomial& q);

	UnivariatePolynomial squareFreePart() const;
	
	Coefficient evaluate(const Coefficient& value) const;

	/**
	 * Notice, Cauchy bounds are only defined for polynomials over fields.
	 * 
	 * This is 
	 * @return 
	 */
	Coefficient cauchyBound() const;
	Coefficient modifiedCauchyBound() const;

	template<typename C>
	friend bool operator==(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend bool operator!=(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);

	UnivariatePolynomial operator-() const;
	UnivariatePolynomial& operator+=(const Coefficient& rhs);
	/**
	 * @param rhs A univariate polynomial over the same variable.
	 * @return 
	 */
	UnivariatePolynomial& operator+=(const UnivariatePolynomial& rhs);
	UnivariatePolynomial& operator-=(const Coefficient& rhs);
	/**
	 * @param rhs A univariate polynomial over the same variable.
	 * @return 
	 */
	UnivariatePolynomial& operator-=(const UnivariatePolynomial& rhs);
	UnivariatePolynomial& operator*=(const Coefficient& rhs);
	/**
	 * @param rhs A univariate polynomial over the same variable.
	 * @return 
	 */
	UnivariatePolynomial& operator*=(const UnivariatePolynomial& rhs);
	/**
	 * Only defined for field-coefficients.
	 * @param rhs
	 * @return 
	 */
	UnivariatePolynomial& operator/=(const Coefficient& rhs);

	template <typename C>
	friend std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial<C>& rhs);

private:
	Variable mMainVar;
	std::vector<Coefficient> mCoefficients;

	static UnivariatePolynomial gcd_recursive(const UnivariatePolynomial& p, const UnivariatePolynomial& q);
};
}

#include "UnivariatePolynomial.tpp"