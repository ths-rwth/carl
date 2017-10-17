/** 
 * @file  MultivariateFactor.h
 * @author Florian Corzilius
 *
 */

#pragma once

#include "../converter/OldGinacConverter.h"

namespace carl
{
	
template<typename C, typename O, typename P>
class MultivariatePolynomial;
	
/**
 * @param _p A multivariately represented polynomial.
 * @return A factorization of a (depends on settings how fine it is)
 * @see MultivariatePolynomial
 */
template<typename C, typename O, typename P>
Factors<MultivariatePolynomial<C,O,P>> factor(const MultivariatePolynomial<C,O,P>& _p);


template<typename C, typename O, typename P>
Factors<MultivariatePolynomial<C,O,P>> factor(const Term<C>& _t);

template<typename C, typename O, typename P>
Factors<MultivariatePolynomial<C,O,P>> factor(std::shared_ptr<const Monomial> _m);

	
/**
 * A general object for factorization of multivariate polynomials.
 */
template<typename Coeff, typename Ordering= GrLexOrdering, typename Policies = StdMultivariatePolynomialPolicies<>>
class MultivariateFactor
{
    public:
    template<typename C, typename O, typename P>
    friend Factors<MultivariatePolynomial<C,O,P>> factor(const MultivariatePolynomial<C,O,P>& _p);
    
    private:
        
	using Poly = MultivariatePolynomial<Coeff,Ordering,Policies>;
	
	const Poly& mp;
	
	public:
	explicit MultivariateFactor(const Poly& _p):
        mp(_p)
	{}
	
	/**
	 * 
     * @return 
     */
	Factors<Poly> calculate();
		
};

}

#include "MultivariateFactor.tpp"
