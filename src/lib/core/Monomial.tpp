/**
 * @file Monomial.tpp
 * @author Sebastian Junges
 */

#pragma once

// Include for code assistance.
#include "Monomial.h"

namespace arithmetic
{

template <typename Coefficient>
Monomial<Coefficient>::Monomial() :
    mCoefficient(), 
    mExponents()
{
    
}

template <typename Coefficient>
Monomial<Coefficient>::Monomial(const Coefficient& coeff) :
    mCoefficient(coeff), 
    mExponents()
{
    
}

template <typename Coefficient>
Monomial<Coefficient>::Monomial(const Coefficient& coeff, variable v, exponent e) :
    mCoefficient(coeff), 
    mExponents(1, VarExpPair(v,e))
{
    
}


}