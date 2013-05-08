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

// 
// public
// 
Monomial::Monomial(Variable v, exponent e) :
    mExponents(1, VarExpPair(v,e)),
    mTotalDegree(e)
{
    
}

Monomial& Monomial::operator =(const Monomial& rhs)
{
    // Check for self-assignment.
    if(this == &rhs) return *this;
    mExponents = rhs.mExponents;
    mTotalDegree = rhs.mTotalDegree;
    return *this;
}


CompareResult Monomial::compareLexical(const Monomial& lhs, const Monomial& rhs)
{
    return lexicalCompare(lhs,rhs);
}


CompareResult Monomial::compareGradedLexical(const Monomial& lhs, const Monomial& rhs)
{
    if(lhs.mTotalDegree < rhs.mTotalDegree) return CompareResult::LESS;
    if(lhs.mTotalDegree > rhs.mTotalDegree) return CompareResult::GREATER;
    return lexicalCompare(lhs, rhs);
}

bool operator==(const Monomial& lhs, const Monomial& rhs)
{
    if(lhs.mTotalDegree != rhs.mTotalDegree) return false;
    else return lhs.mExponents == rhs.mExponents;
}

bool operator==(const Monomial& lhs, Variable::Arg rhs)
{
    // TODO think about reordering based on what is most probable.
    if(lhs.mTotalDegree != 1) return false;
    if(lhs.mExponents[0].var == rhs) return true;
    else return false;
}

bool operator==(const Variable& lhs, const Monomial& rhs)
{
    return rhs == lhs;
}

bool operator!=(const Monomial& lhs, const Monomial& rhs)
{
    return !(lhs == rhs);
}

bool operator!=(const Monomial& lhs, const Variable& rhs)
{
    return !(lhs == rhs);
}
bool operator!=(const Variable& lhs, const Monomial& rhs)
{
    return !(lhs == rhs);
}

Monomial& Monomial::operator*=(Variable::Arg v)
{
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

Monomial& Monomial::operator*=(const Monomial& rhs)
{
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
        
const Monomial operator*(const Monomial& lhs, Variable::Arg rhs )
{
    // Note that this implementation is not optimized yet!
    Monomial result(lhs);
    result *= rhs;
    return result;
}

const Monomial operator*(Variable::Arg lhs, const Monomial& rhs )
{
    return rhs * lhs;
}

const Monomial operator*(const Monomial& lhs, const Monomial& rhs )
{
    // Note that this implementation is not optimized yet!
    Monomial result(lhs);
    result *= rhs;
    return result;
}
        
std::ostream& operator <<( std::ostream& os, const Monomial& rhs )
{
    for( VarExpPair vp : rhs.mExponents )
    {
        os << vp;
    }
    return os;
}

// 
// private:
// 
CompareResult Monomial::lexicalCompare(const Monomial& lhs, const Monomial& rhs)
{
    exponents_cIt lhsit = lhs.mExponents.begin( );
    exponents_cIt rhsit = rhs.mExponents.begin( );
    exponents_cIt lhsend = lhs.mExponents.end( );
    exponents_cIt rhsend = rhs.mExponents.end( );

    while( lhsit != lhsend )
    {
        if( rhsit == rhsend )
            return CompareResult::GREATER;
        //which variable occurs first
        if( lhsit->var == rhsit->var )
        {
            //equal variables
            if( lhsit->exp < rhsit->exp )
                return CompareResult::LESS;
            if( lhsit->exp > rhsit->exp )
                return CompareResult::GREATER;
        }
        else
        {
            return (lhsit->var < rhsit->var ) ? CompareResult::GREATER : CompareResult::LESS;
        }
        ++lhsit;
        ++rhsit;
    }
    if( rhsit == rhsend )
        return CompareResult::EQUAL;
    return CompareResult::LESS;
}


}