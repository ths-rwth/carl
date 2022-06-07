/**
 * @file MultivariatePolynomialPolicy.h 
 * @ingroup multirp
 * @author Sebastian Junges
 */

#pragma once

#include "MonomialOrdering.h"
#include "MultivariatePolynomialAdaptors/PolynomialAllocator.h"
#include "MultivariatePolynomialAdaptors/ReasonsAdaptor.h"

namespace carl
{
    /**
     * The default policy for polynomials. 
	 * @ingroup multirp
     */
	template<typename ReasonsAdaptor = NoReasons, typename Allocator = NoAllocator>
    struct StdMultivariatePolynomialPolicies : public ReasonsAdaptor
    {
		
		
        /**
         * Linear searching means that we search linearly for a term instead of applying e.g. binary search.
         * Although the worst-case complexity is worse, for polynomials with a small nr of terms, this should be better.
         */
        static const bool searchLinear = true;
		
		// Easy access.
		static const bool has_reasons = ReasonsAdaptor::has_reasons;
    };
	
}
