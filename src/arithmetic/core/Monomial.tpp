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



template <typename Coefficient>
CompareResult Monomial<Coefficient>::compareGradedLexical()
{
    
}

template <typename Coefficient>
Monomial<Coefficient>& Monomial<Coefficient>::operator*=(Variable::Arg v)
{
    // We use canonical representation for the zero.
    if(mCoefficient == 0)
    {
        return *this;
    }
    // Linear, as we expect small monomials.
    for(exponents_it it = mExponents.begin(); it != mExponents.end(); ++it)
    {
        // Variable is present
        if(*it == v)
        {
            ++(it->exp);
            return *this;
        }
        // Variable is not present, we have to insert v.
        if(*it > v)
        {
            mExponents.emplace(it,v);
        }
    }
    // Variable was not inserted, insert at end.
    mExponents.emplace_back(v);
    return *this;
}

template <typename Coefficient>
Monomial<Coefficient>& Monomial<Coefficient>::operator*=(const Coefficient& rhs)
{
    if(rhs == 0)
    {
        mCoefficient = 0;
        mExponents.clear();
        mTotalDegree = 0;
    }
    else
    {
        mCoefficient *= rhs;
    }
    return *this;
}

template <typename C>
std::ostream& operator <<( std::ostream& os, const Monomial<C>& rhs )
{
    os << rhs.mCoefficient;
    std::copy(rhs.mExponents.begin(), rhs.mExponents.end(),std::ostream_iterator<std::string>(os));
    return os;
}

}