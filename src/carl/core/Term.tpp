#pragma once

// for code assistance
#include "Term.h"
#include "Monomial_derivative.h"
#include "Monomial_substitute.h"

namespace carl
{
template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c) :
    mCoeff(c), mMonomial()
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
Term<Coefficient>::Term(const Coefficient& c, const Monomial* m) :
    mCoeff(c), mMonomial(std::shared_ptr<const Monomial>(m))
{
    
}

template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c, Variable::Arg v, unsigned exponent) :
mCoeff(c), mMonomial(std::make_shared<Monomial>(Monomial(v, exponent)))
{
    
}

template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c, const Monomial& m)
: mCoeff(c)
{
    if (c != 0) mMonomial = std::make_shared<const Monomial>(m);
}

template<typename Coefficient>
Term<Coefficient>::Term(const Coefficient& c, const std::shared_ptr<const Monomial>& m)
: mCoeff(c)
{
    if(c != 0) mMonomial = m;
}


template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::dividedBy(const Coefficient& c) const
{
    assert(c != 0);
    return new Term(mCoeff / c, mMonomial);
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::dividedBy(Variable::Arg v) const
{
    if(mMonomial)
    {
        Monomial* div = mMonomial->dividedBy(v);
        if(div != nullptr)
        {
            return new Term<Coefficient>(mCoeff, div);
        }   
    }
    return nullptr;    
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::dividedBy(const Monomial& m) const
{
    if(mMonomial)
    {
        Monomial* div = mMonomial->dividedBy(m);
        if(div != nullptr)
        {
            return new Term<Coefficient>(mCoeff, div);
        }   
    }
    return nullptr;  
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::dividedBy(const Term& t) const
{
    assert(t.mCoeff != 0);
    if(mMonomial)
    {
        if(!t.mMonomial)
        {
            // Term is just a constant.
            return new Term<Coefficient>(mCoeff / t.mCoeff, mMonomial);
        }
        Monomial* div = mMonomial->dividedBy(*(t.mMonomial));
        if(div != nullptr)
        {
            return new Term<Coefficient>(mCoeff / t.mCoeff, div);
        }   
    } 
    else if(!t.mMonomial)
    {
       // Division of constants.
        return new Term<Coefficient>(mCoeff / t.mCoeff);
    }
    return nullptr;  
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::derivative(Variable::Arg v) const
{
    Term<Coefficient>* t = mMonomial->derivative<Coefficient>(v);
    *t *= mCoeff;
    return t;
}

template<typename Coefficient>
Term<Coefficient>* Term<Coefficient>::substitute(const std::map<Variable,Coefficient>& substitutions) const
{
	return mMonomial->substitute<Coefficient>(substitutions, coeff());
}


template<typename Coefficient>
Term<Coefficient> Term<Coefficient>::calcLcmAndDivideBy(const Monomial& m) const
{
	return Term(1, monomial()->calcLcmAndDivideBy(m));
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
const Term<Coefficient> Term<Coefficient>::operator-() const
{
    return Term<Coefficient>(-mCoeff,mMonomial);
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
Term<Coefficient>& Term<Coefficient>::operator*=(const Monomial& rhs)
{
    if(mCoeff == 0) return *this;
    
    if(mMonomial)
    {
        mMonomial = std::make_shared<const Monomial>((*mMonomial) * (rhs));
        
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
            mMonomial = std::make_shared<const Monomial>((*mMonomial) * (*rhs.mMonomial));
        }
    }
    else
    {
        mMonomial = rhs.mMonomial;
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
const Term<Coeff> operator*(const Term<Coeff>& lhs, int rhs)
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
    if(rhs.mMonomial)
    {
        return os << rhs.mCoeff << *rhs.mMonomial;
    }
    else 
    {
        return os << rhs.mCoeff;
    }
}


}