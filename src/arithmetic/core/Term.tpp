#pragma once

// for code assistance
#include "Term.h"

namespace arithmetic
{
template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c) :
    mCoeff(c)
{
    
}
template<typename Coefficient>
Term<Coefficient>::Term(Variable::Arg v) :
    mCoeff(1), mMonomial(new Monomial(v))
{
    
}

template<typename Coefficient>
Term<Coefficient>::Term(const Monomial& m) :
    mCoeff(1), mMonomial(new Monomial(m))
{
    
}

template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c, std::shared_ptr<const Monomial> m)
: mCoeff(c)
{
    if(c != 0) mMonomial(m);
}

template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, const Term<Coeff>& rhs)
{
    return (lhs.mCoeff == rhs.mCoeff) &&
            ((!lhs.mMonomial && !rhs.mMonomial) || 
             (lhs.mMonomial && rhs.mMonomial && *lhs.mMonomial == *rhs.mMonomial));
}
template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, const Coeff& rhs)
{
    return !lhs.mMonomial && lhs.mCoeff == rhs;
}
template<typename Coeff>
bool operator==(const Coeff& lhs, const Term<Coeff>& rhs)
{
    return rhs == lhs;
}
template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, Variable::Arg rhs)
{
    return lhs.mMonomial && *(lhs.mMonomial) == rhs;
}
template<typename Coeff>
bool operator==(Variable::Arg lhs, const Term<Coeff>& rhs)
{
    return rhs == lhs;
}
template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, const Monomial& rhs)
{
    return lhs.mMonomial && *(lhs.mMonomial) == rhs;
}
template<typename Coeff>
bool operator==(const Monomial& lhs, const Term<Coeff>& rhs)
{
    return (rhs == lhs);
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, const Term<Coeff>& rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, const Coeff& rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(const Coeff& lhs, const Term<Coeff>& rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, Variable::Arg rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(Variable::Arg lhs, const Term<Coeff>& rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, const Monomial& rhs)
{
    return !(lhs == rhs);
}
template<typename Coeff>
bool operator!=(const Monomial& lhs, const Term<Coeff>& rhs)
{
    return !(lhs == rhs);
}

template<typename Coefficient>
Term<Coefficient>& Term<Coefficient>::operator*=(const Coefficient& rhs)
{
    if(rhs == 0) 
    {
        clear();
        return *this;
    }
    assert(mCoeff == 0 || mCoeff * rhs != 0);
    mCoeff *= rhs;
    return *this;
}
template<typename Coefficient>
Term<Coefficient>& Term<Coefficient>::operator*=(Variable::Arg rhs)
{
    if(mCoeff == 0)
    {
        return *this;
    }
    if(mMonomial)
    {
        mMonomial = std::make_shared<const Monomial>(*mMonomial * rhs);
    }
    else
    {
        mMonomial = std::make_shared<const Monomial>(rhs);
    }
    return *this;
}
template<typename Coefficient>
Term<Coefficient>& Term<Coefficient>::operator*=(const Term& rhs)
{
    if(mCoeff == 0) return *this;
    if(rhs.mCoeff == 0) 
    {
        clear();
        return *this;
    }
    
    if(mMonomial)
    {
        if(rhs.mMonomial)
        {
            mMonomial = std::make_shared<const Monomial>(*mMonomial * *(rhs.mMonomial));
        }
    }
    else
    {
        mMonomial = std::make_shared<const Monomial>(rhs.mMonomial);
    }
    return *this;   
}

template<typename Coeff>
const Term<Coeff> operator*(const Term<Coeff>& lhs, const Term<Coeff>& rhs)
{
    Term<Coeff> result(lhs);
    result *= rhs;
    return result;
}
template<typename Coeff>
const Term<Coeff> operator*(const Term<Coeff>& lhs, const Coeff& rhs)
{
    return Term<Coeff>(lhs.mCoeff * rhs, lhs.mMonomial);
}
template<typename Coeff>
const Term<Coeff> operator*(const Coeff& lhs, const Term<Coeff>& rhs)
{
    return rhs * lhs;
}
template<typename Coeff>
const Term<Coeff> operator*(const Term<Coeff>& lhs, Variable::Arg rhs)
{
    Term<Coeff> result(lhs);
    result *= rhs;
    return result;
}
template<typename Coeff>
const Term<Coeff> operator*(Variable::Arg lhs, const Term<Coeff>& rhs)
{
    return rhs * lhs;
}
template<typename Coeff>
const Term<Coeff> operator*(const Coeff& lhs, Variable::Arg rhs)
{
    return Term<Coeff>(lhs, Monomial(rhs));
}
template<typename Coeff>
const Term<Coeff> operator*(Variable::Arg lhs, const Coeff& rhs)
{
    return rhs * lhs;
}
template<typename Coeff>
const Term<Coeff> operator*(const Coeff& lhs, const Monomial& rhs)
{
    return Term<Coeff>(lhs, rhs);
}
template<typename Coeff>
const Term<Coeff> operator*(const Monomial& lhs, const Coeff& rhs)
{
    return rhs * lhs;
}

template<typename Coeff>
std::ostream& operator<<(std::ostream& os, const Term<Coeff>& rhs)
{
    return os << rhs.mCoeff << rhs.mMonomial;
}


}