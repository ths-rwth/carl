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
Monomial<Coefficient>& Monomial<Coefficient>::operator =(const Monomial& rhs)
{
    // Check for self-assignment.
    if(this == &rhs) return *this;
    mCoefficient = rhs.mCoefficient;
    mExponents = rhs.mExponents;
    mTotalDegree = rhs.mTotalDegree;
    return *this;
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

template<typename Coefficient>
Monomial<Coefficient>& Monomial<Coefficient>::operator*=(const Monomial<Coefficient>& rhs)
{
    // Multiplying 0 with something does not change anything.
    if(mCoefficient == 0) return *this;
    // Multiplying with 0 makes zero.
    if(rhs.mCoefficient == 0)
    {
        mCoefficient = 0;
        mExponents.clear();
        mTotalDegree = 0;
        return *this;
    }
    mCoefficient *= rhs.mCoefficient;
    if(rhs.mTotalDegree == 0) return *this;
    mTotalDegree += rhs.mTotalDegree;
    
    // Linear, as we expect small monomials.
    exponents_cIt itright = rhs.mExponents.begin();
    for(exponents_it itleft = mExponents.begin(); itleft != mExponents.end(); ++itleft)
    {
        // Everything is inserted.
        if(itright == rhs.mExponents.end())
        {
            return *this;
        }
        // Variable is present in both monomials.
        if(itleft->var == itright->var)
        {
            itleft->exp += itright->exp;
            ++itright;
        }
        // Variable is not present in lhs, we have to insert var-exp pair from rhs.
        else if(itleft->var > itright->var) 
        {
            mExponents.insert(itleft,*itright);
            ++itright;
        }        
    }
    // Insert remainder of rhs.
    mExponents.insert(mExponents.end(), itright, rhs.mExponents.end());
    return *this;
}
        
template<typename C>
const Monomial<C> operator*(const C& lhs, Variable::Arg rhs)
{
    // Note that this implementation is not optimized yet!
    return Monomial<C>(lhs, rhs);
}
template<typename C>
const Monomial<C> operator*(Variable::Arg lhs, const C& rhs)
{
    return rhs * lhs;
}
template<typename C>
const Monomial<C> operator*(const Monomial<C>& lhs, Variable::Arg rhs )
{
    // Note that this implementation is not optimized yet!
    Monomial<C> result(lhs);
    result *= rhs;
    return result;
}
template<typename C>
const Monomial<C> operator*(Variable::Arg lhs, const Monomial<C>& rhs )
{
    return rhs * lhs;
}
template<typename C>
const Monomial<C> operator*(const Monomial<C>& lhs, const C& rhs )
{
    // Note that this implementation is not optimized yet!
    Monomial<C> result(lhs);
    result *= rhs;
    return result;
}
template<typename C>
const Monomial<C> operator*(const C& lhs, const Monomial<C>& rhs)
{
    return lhs * rhs;
}
template<typename C>
const Monomial<C> operator*(const Monomial<C>& lhs, const Monomial<C>& rhs )
{
    // Note that this implementation is not optimized yet!
    Monomial<C> result(lhs);
    result *= rhs;
    return result;
}
        

template <typename C>
std::ostream& operator <<( std::ostream& os, const Monomial<C>& rhs )
{
    os << rhs.mCoefficient;
    for( VarExpPair vp : rhs.mExponents )
    {
        os << vp;
    }
    return os;
}

}