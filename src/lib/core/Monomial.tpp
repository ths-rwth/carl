/**
 * @file Monomial.tpp
 * @author Sebastian Junges
 */

#pragma once

// Include for code assistance.
#include "Monomial.h"
#include <iterator>

namespace arithmetic
{

template <typename Coefficient>
Monomial<Coefficient>::Monomial() :
    mCoefficient(0), 
    mExponents(),
    mTotalDegree(0)
{
    
}

template <typename Coefficient>
Monomial<Coefficient>::Monomial(const Coefficient& coeff) :
    mCoefficient(coeff), 
    mExponents(),
    mTotalDegree(0)
{
    
}

template <typename Coefficient>
Monomial<Coefficient>::Monomial(const Coefficient& coeff, Variable v, exponent e) :
    mCoefficient(coeff), 
    mExponents(1, VarExpPair(v,e)),
    mTotalDegree(e)
{
    
}

template<typename C>
std::ostream& operator <<( std::ostream& os, const Monomial<C>& rhs )
{
    os << rhs.mCoefficient;
    std::copy(rhs.mExponents.begin(), rhs.mExponents.end(),std::ostream_iterator<std::string>(os));
    return os;
}

}