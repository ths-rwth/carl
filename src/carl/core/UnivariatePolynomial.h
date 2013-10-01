/**
 * @file UnivariatePolynomial.h 
 * @author Sebastian Junges
 */

#pragma once
#include <map>

#include "Variable.h"
#include "VariableInformation.h"
#include "Polynomial.h"

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
		return (unsigned)mCoefficients.size();
	}
	
	const Coefficient& lcoeff() const
	{
		return mCoefficients.back();
	}
	
	UnivariatePolynomial derivative(unsigned nth=1) const;
	
	UnivariatePolynomial reduce(const UnivariatePolynomial& divisor) const;
	
	static UnivariatePolynomial gcd(const UnivariatePolynomial& p, const UnivariatePolynomial& q);
	
	//friend bool operator==(const UnivariatePolynomial& lhs, const UnivariatePolynomial& rhs);
	
	UnivariatePolynomial& operator+=(const Coefficient& rhs);
	/**
	 * 
     * @param rhs A univariate polynomial over the same variable.
     * @return 
     */
	UnivariatePolynomial& operator+=(const UnivariatePolynomial& rhs);
	
	
	template <typename C>
	friend std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial<C>& rhs);

private:
	Variable mMainVar;
    std::vector<Coefficient> mCoefficients;
	
	static UnivariatePolynomial gcd_recursive(const UnivariatePolynomial& p, const UnivariatePolynomial& q);
};
}

#include "UnivariatePolynomial.tpp"