/**
 * @file Polynomial.h 
 * @author Sebastian Junges
 */

#pragma once

#include <vector>
#include "PolynomialPolicy.h"
#include "Monomial.h"



namespace arithmetic
{
    /**
     *  The general-purpose multivariate polynomial class.
     */
    template<typename Coeff, typename Policy = StdPolynomialPolicy>
    class Polynomial
    {
    protected:
        /// A vector of all monomials
        std::vector<Monomial<Coeff>> mTerms;
    public:
        Polynomial() = default;
        Polynomial(const Monomial& m);
        ~Polynomial() = default;
        
        template <typename C,P>
        friend std::ostream& operator <<( std::ostream& os, const Polynomial<C,P>& rhs );
    };
}

#include "Polynomial.tpp"
