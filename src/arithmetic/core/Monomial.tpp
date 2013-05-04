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
Monomial<Coefficient>::Monomial(const Coefficient& coeff) :
    mCoefficient(coeff)
{
    
}
template <typename Coefficient>
Monomial<Coefficient>::Monomial(Variable v, exponent e) :
    mCoefficient(1),
    mExponents(1, VarExpPair(v,e)),
    mTotalDegree(e)
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

template<typename C1, typename C2>
bool operator==(const Monomial<C1>& lhs, const Monomial<C2>& rhs)
{
    if(lhs.mTotalDegree != rhs.mTotalDegree || lhs.mCoefficient != rhs.mCoefficient) return false;
    else return lhs.mExponents == rhs.mExponents;
}
template<typename C>
bool operator==(const Monomial<C>& lhs, const C& rhs)
{
    // Degree is just a machine word, so it is the fastest check.
    if(lhs.mTotalDegree == 0 && lhs.mCoefficient == rhs) return true;
    else return false;
}
template<typename C>
bool operator==(const Monomial<C>& lhs, Variable::Arg rhs)
{
    // TODO think about reordering based on what is most probable.
    if(lhs.mCoefficient != 1 || lhs.mTotalDegree != 1) return false;
    if(lhs.mExponents[0].var == rhs) return true;
    else return false;
}

template<typename C>
bool operator==(const C& lhs, const Monomial<C>& rhs)
{
    return rhs == lhs;
}
template<typename C>
bool operator==(const Variable& lhs, const Monomial<C>& rhs)
{
    return rhs == lhs;
}

template<typename C1, typename C2>
bool operator!=(const Monomial<C1>& lhs, const Monomial<C2>& rhs)
{
    return !(lhs == rhs);
}
template<typename C>
bool operator!=(const Monomial<C>& lhs, const C& rhs)
{
    return !(lhs == rhs);
}
template<typename C>
bool operator!=(const C& lhs, const Monomial<C>& rhs)
{
    return !(lhs == rhs);
}
template<typename C>
bool operator!=(const Monomial<C>& lhs, const Variable& rhs)
{
    return !(lhs == rhs);
}
template<typename C>
bool operator!=(const Variable& lhs, const Monomial<C>& rhs)
{
    return !(lhs == rhs);
}

template <typename Coefficient>
Monomial<Coefficient>& Monomial<Coefficient>::operator*=(Variable::Arg v)
{
    // We use canonical representation for the zero.
    if(mCoefficient == 0)
    {
        return *this;
    }
    ++mTotalDegree;
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