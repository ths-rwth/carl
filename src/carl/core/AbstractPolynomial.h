/**
 * @file AbstractPolynomial.h 
 * @author Sebastian Junges
 */

#pragma once

namespace carl
{
    /**
     *  Abstract base class for general-purpose polynomials.
     */
    class AbstractPolynomial
    {
        virtual AbstractPolynomial() = 0;
    };
}
