/**
 * @file UnivariatePolynomial.h 
 * @author Sebastian Junges
 */

#pragma once
#include <map>
#include <vector>

#include "Variable.h"
#include "VariableInformation.h"
#include "Polynomial.h"
#include "DivisionResult.h"
#include "../numbers/numbers.h"
#include "../numbers/GFNumber.h"

#include "logging.h"
#include "../util/SFINAE.h"

namespace carl
{

template<typename C, typename O, typename P>
class MultivariatePolynomial;
	
template<typename Coefficient>
class UnivariatePolynomial : public Polynomial
{
	
	template<class T> friend class UnivariatePolynomial; 
private:
	Variable mMainVar;
	std::vector<Coefficient> mCoefficients;

public:
	UnivariatePolynomial(Variable::Arg mainVar);
	UnivariatePolynomial(Variable::Arg mainVar, const Coefficient& coeff, unsigned degree=0);
	UnivariatePolynomial(Variable::Arg mainVar, std::initializer_list<Coefficient> coefficients);
	UnivariatePolynomial(Variable::Arg mainVar, const std::vector<Coefficient>& coefficients);
	UnivariatePolynomial(Variable::Arg mainVar, std::vector<Coefficient>&& coefficients);
	UnivariatePolynomial(Variable::Arg mainVar, const std::map<unsigned, Coefficient>& coefficients);
//	UnivariatePolynomial(Variable::Arg mainVar, const VariableInformation<true, Coefficient>& varinfoWithCoefficients);

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
	
	template<typename C=Coefficient, EnableIf<is_instantiation_of<GFNumber, C>> = dummy>
	UnivariatePolynomial one() const
	{
		return UnivariatePolynomial(mMainVar, C(1, lcoeff().gf()));
	}
	template<typename C=Coefficient, DisableIf<is_instantiation_of<GFNumber, C>> = dummy>
	UnivariatePolynomial one() const
	{
		return UnivariatePolynomial(mMainVar, (C)1);
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

	const std::vector<Coefficient>& coefficients() const
	{
		return mCoefficients;
	}

	const Variable& mainVar() const
	{
		return mMainVar;
	}
	/**
	 * 
     * @return copr
     */
    Coefficient coprimeFactor() const;
    
	template<typename Integer>
	UnivariatePolynomial<Integer> coprimeCoefficients() const;
	
	template<typename C = Coefficient, EnableIf<is_field<C>> = dummy>
	UnivariatePolynomial normalized() const;
	template<typename C = Coefficient, DisableIf<is_field<C>> = dummy>
	UnivariatePolynomial normalized() const;
	
	
	UnivariatePolynomial derivative(unsigned nth = 1) const;

	
	UnivariatePolynomial reduce(const UnivariatePolynomial& divisor) const;
	
	
	DivisionResult<UnivariatePolynomial> divide(const UnivariatePolynomial& divisor) const;
	bool divides(const UnivariatePolynomial&) const;
	
	UnivariatePolynomial& mod(const Coefficient& modulus);
	UnivariatePolynomial mod(const Coefficient& modulus) const;
	static UnivariatePolynomial gcd(const UnivariatePolynomial& a, const UnivariatePolynomial& b);
	static UnivariatePolynomial extended_gcd(const UnivariatePolynomial& a, const UnivariatePolynomial& b,
											 UnivariatePolynomial& s, UnivariatePolynomial& t);

	UnivariatePolynomial squareFreePart() const;
	
	Coefficient evaluate(const Coefficient& value) const;
	
	template<typename SubstitutionType, typename C = Coefficient, EnableIf<is_instantiation_of<MultivariatePolynomial, C>> = dummy>
	UnivariatePolynomial<typename CoefficientRing<Coefficient>::type> evaluateCoefficient(const std::map<Variable, SubstitutionType>&) const
	{
		
	}
	template<typename SubstitutionType, typename C = Coefficient, DisableIf<is_instantiation_of<MultivariatePolynomial, C>> = dummy>
	UnivariatePolynomial<Coefficient> evaluateCoefficient(const std::map<Variable, SubstitutionType>&) const
	{
		// TODO check behaviour here. 
		return *this;
	}
	
	template<typename T = Coefficient, EnableIf<has_normalize<T>> = dummy>
	UnivariatePolynomial& normalizeCoefficients()
	{
		static_assert(std::is_same<T,Coefficient>::value, "No template parameters should be given");
		for(Coefficient& c : mCoefficients)
		{
			c.normalize();
		}
		return *this;
	}
	template<typename T = Coefficient, DisableIf<has_normalize<T>> = dummy>
	UnivariatePolynomial& normalizeCoefficients()
	{
		static_assert(std::is_same<T,Coefficient>::value, "No template parameters should be given");
		LOGMSG_WARN("carl.core", "normalize not possible");
		return *this;
	}
	/**
	 * Works only from rationals, if the numbers are already integers.
     * @return 
     */
	template<typename C=Coefficient, EnableIf<is_instantiation_of<GFNumber, C>> = dummy>
	UnivariatePolynomial<typename IntegralT<Coefficient>::type> toIntegerDomain() const;
	template<typename C=Coefficient, DisableIf<is_instantiation_of<GFNumber, C>> = dummy>
	UnivariatePolynomial<typename IntegralT<Coefficient>::type> toIntegerDomain() const;
	
	UnivariatePolynomial<GFNumber<typename IntegralT<Coefficient>::type>> toFiniteDomain(const GaloisField<typename IntegralT<Coefficient>::type>* galoisField) const;

	/**
	 * Notice, Cauchy bounds are only defined for polynomials over fields.
	 * 
	 * This is 
	 * @return 
	 */
	Coefficient cauchyBound() const;
	Coefficient modifiedCauchyBound() const;
    
    std::map<UnivariatePolynomial, unsigned> factorization() const;
    
    template<typename Integer>
    static UnivariatePolynomial excludeLinearFactors(const UnivariatePolynomial& _poly, std::map<UnivariatePolynomial, unsigned>& _linearFactors, const Integer& maxNum );
    
    Coefficient syntheticDivision(const Coefficient& _zeroOfDivisor);
	std::map<unsigned, UnivariatePolynomial> squareFreeFactorization() const;

	template<typename C>
	friend bool operator==(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend bool operator==(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend bool operator==(const UnivariatePolynomial<C>& lhs, const C& rhs);
	template<typename C>
	friend bool operator!=(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend bool operator<(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);

	UnivariatePolynomial operator-() const;
	UnivariatePolynomial& operator+=(const Coefficient& rhs);
	/**
	 * @param rhs A univariate polynomial over the same variable.
	 * @return 
	 */
	UnivariatePolynomial& operator+=(const UnivariatePolynomial& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator+(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator+(const UnivariatePolynomial<C>& lhs, const C& rhs);
	
	UnivariatePolynomial& operator-=(const Coefficient& rhs);
	/**
	 * @param rhs A univariate polynomial over the same variable.
	 * @return 
	 */
	UnivariatePolynomial& operator-=(const UnivariatePolynomial& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator-(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator-(const UnivariatePolynomial<C>& lhs, const C& rhs);
	
	
	UnivariatePolynomial& operator*=(const Coefficient& rhs);
	template<typename I = Coefficient, DisableIf<std::is_same<Coefficient, I>>...>
	UnivariatePolynomial& operator*=(const typename IntegralT<Coefficient>::type& rhs);
	/**
	 * @param rhs A univariate polynomial over the same variable.
	 * @return 
	 */
	UnivariatePolynomial& operator*=(const UnivariatePolynomial& rhs);
	
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const C& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const C& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const typename IntegralT<C>::type& lhs, const UnivariatePolynomial<C>& rhs);
	template<typename C>
	friend UnivariatePolynomial<C> operator*(const UnivariatePolynomial<C>& lhs, const typename IntegralT<C>::type& rhs);
	template<typename C, typename O, typename P>
	friend UnivariatePolynomial<MultivariatePolynomial<C,O,P>> operator*(const UnivariatePolynomial<MultivariatePolynomial<C,O,P>>& lhs, const C& rhs);
	template<typename C, typename O, typename P>
	friend UnivariatePolynomial<MultivariatePolynomial<C,O,P>> operator*(const C& lhs, const UnivariatePolynomial<MultivariatePolynomial<C,O,P>>& rhs);
	
	
	/**
	 * Only defined for field-coefficients.
	 * @param rhs
	 * @return 
	 */
	UnivariatePolynomial& operator/=(const Coefficient& rhs);
	
	template<typename C>
	friend UnivariatePolynomial<C> operator/(const UnivariatePolynomial<C>& lhs, const C& rhs);
	
	template <typename C>
	friend std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial<C>& rhs);
	
	
	private:
	static UnivariatePolynomial gcd_recursive(const UnivariatePolynomial& p, const UnivariatePolynomial& q);
	void stripLeadingZeroes() 
	{
		while(!isZero() && lcoeff() == 0)
		{
			mCoefficients.pop_back();
		}
	}
};
}

#include "UnivariatePolynomial.tpp"
