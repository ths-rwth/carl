/** 
 * @file  MultivariateGCD.h
 * @ingroup multirp
 * @ingroup gcd
 * @author Sebastian Junges
 *
 */

#pragma once

#include "MultivariatePolynomial.h"
#include "UnivariatePolynomial.h"

namespace carl
{
/**
 * The result of a gcd calculation, as with the extended gcd.
 * @ingroup gcd
 */	
template<typename Coeff, typename Ordering= GrLexOrdering, typename Policies = StdMultivariatePolynomialPolicies<>>
struct GCDResult
{
	typedef MultivariatePolynomial<Coeff,Ordering,Policies> Polynomial;

	Polynomial AdivG;
	Polynomial BdivG;
	Polynomial gcd;
};
	
/**
 * A general object for gcd calculation of multivariate gcds.
 * @ingroup gcd
 * @ingroup multirp
 */
template<typename GCDCalculation, typename Coeff, typename Ordering= GrLexOrdering, typename Policies = StdMultivariatePolynomialPolicies<>>
class MultivariateGCD : GCDCalculation
{
	typedef GCDResult<Coeff,Ordering,Policies> Result;
	typedef MultivariatePolynomial<Coeff,Ordering,Policies> Polynomial;
	typedef UnivariatePolynomial<MultivariatePolynomial<Coeff,Ordering,Policies>> UnivReprPol;
	typedef UnivariatePolynomial<Coeff> UnivPol;
	
	const Polynomial& mp1;
	const Polynomial& mp2;
	
	public:
	MultivariateGCD(const MultivariatePolynomial<Coeff,Ordering,Policies>& p1,const MultivariatePolynomial<Coeff,Ordering,Policies>& p2)
	: mp1(p1), mp2(p2), GCDCalculation()
	{
		
	}
	
	/**
	 * 
     * @param approx
     * @return 
     */
	Result calculate(bool approx=true)
	{
		assert(approx);
		
		// We start with some trivial cases.
		if(mp1 == (Coeff)1 || mp2 == (Coeff)1) return {Polynomial((Coeff)1), Polynomial((Coeff)1), Polynomial((Coeff)1)};
		if(mp1.nrTerms() == 1 && mp2.nrTerms() == 1)
		{
			//return Polynomial(Term::gcd(*mp1.lterm(), *mp2.lterm()));
		}
		
		// And we do some simplifications for the input.
		// In order to do so, we gather some information about the polynomials, as we most certainly need them later on.
		
		// We check for mutual trivial factorizations.
		
		// And we check for linearly appearing variables. Notice that ay + b is irreducible and thus,
		// gcd(p, ay + b) is either ay + b or 1.
		
		Variable x = getMainVar(mp1, mp2);
		UnivReprPol A = mp1.toUnivariatePolynomial(x);
		UnivReprPol B = mp2.toUnivariatePolynomial(x);
	}
	
	private:
	/**
	 * Given the two polynomials, find a suitable main variable for gcd.
     * @param p1
     * @param p2
     * @return NoVariable if intersection is empty, otherwise some variable v which is in p1 and p2.
     */
	Variable getMainVar(const Polynomial p1, const Polynomial p2) const
	{
		// TODO find good heuristic.
		std::set<Variable> common;
		std::set<Variable> v1 = p1.gatherVariables();
		std::set<Variable> v2 = p2.gatherVariables();
		
		std::set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),
                  std::inserter(common,common.begin()));
		if(common.empty())
		{
			return Variable::NO_VARIABLE;
		}
		else
		{
			return *common.begin();
		}
		
	}	
		
};

/**
 * @param a
 * @param b
 * @return 
 * @ingroup gcd
 * @ingroup multirp
 * @see MultivariatePolynomial
 * @see MultivariateGCD
 */
template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> gcd(const MultivariatePolynomial<C,O,P>& a, const MultivariatePolynomial<C,O,P>& b);
	
	
}
#include "MultivariateGCD.tpp"