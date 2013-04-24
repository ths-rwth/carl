/**
 * @file Polynomial.h 
 * @author Sebastian Junges
 */

#pragma once

#include "AbstractPolynomial.h"
#include "PolynomialPolicy.h"
#include "Monomial.h"

namespace arithmetic
{
    /**
     *  The general-purpose multivariate polynomial class.
     */
    template<typename Coeff, typename Policy = StdPolynomialPolicy>
    class Polynomial : public AbstractPolynomial
    {
        
    };
}
