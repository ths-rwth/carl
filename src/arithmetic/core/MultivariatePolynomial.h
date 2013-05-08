/**
 * @file Polynomial.h 
 * @author Sebastian Junges
 */

#pragma once

#include <vector>
#include "PolynomialPolicy.h"
#include "Term.h"



namespace arithmetic
{
    /**
     *  The general-purpose multivariate polynomial class.
     */
    template<typename Coeff, typename Policy = StdPolynomialPolicy>
    class MultivariatePolynomial
    {
    protected:
        /// A vector of all monomials
        std::vector<shared_ptr<Term<Coeff>>> mTerms;
    public:
        MultivariatePolynomial() = default;
        explicit MultivariatePolynomial(const Monomial& m);
        
        template <typename C,P>
        friend std::ostream& operator <<( std::ostream& os, const MultivariatePolynomial<C,P>& rhs );
    };
}

#include "Polynomial.tpp"
