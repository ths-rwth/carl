/**
 * @file AbstractPolynomial.h 
 * @author Sebastian Junges
 */

#pragma once

#include "MonomialOrdering.h"

namespace carl
{
    /**
     * The default policy for polynomials. 
     */
    struct StdMultivariatePolynomialPolicy
    {
        /**
         * Linear searching means that we search linearly for a term instead of applying e.g. binary search.
         * Although the worst-case complexity is worse, for polynomials with a small nr of terms, this should be better.
         */
        static const bool searchLinear = true;
    };
	
}
