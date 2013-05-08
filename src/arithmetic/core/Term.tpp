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

template<typename Coeff>
bool operator==(const Term<Coeff>& lhs, const Term<Coeff>& rhs)
{
    return (lhs.mCoeff == rhs.mCoeff) &&
            ((!lhs.mMonomial && !rhs.mMonomial) || 
             (lhs.mMonomial && rhs.mMonomial && *lhs.mMonomial == rhs.mMonomial));
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
    return rhs == lhs;
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, const Term<Coeff>& rhs)
{
    return !lhs == rhs;
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, const Coeff& rhs)
{
    return !lhs == rhs;
}
template<typename Coeff>
bool operator!=(const Coeff& lhs, const Term<Coeff>& rhs)
{
    return !lhs == rhs;
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, Variable::Arg rhs)
{
    return !lhs == rhs;
}
template<typename Coeff>
bool operator!=(Variable::Arg lhs, const Term<Coeff>& rhs)
{
    return !lhs == rhs;
}
template<typename Coeff>
bool operator!=(const Term<Coeff>& lhs, const Monomial& rhs)
{
    return !lhs == rhs;
}
template<typename Coeff>
bool operator!=(const Monomial& lhs, const Term<Coeff>& rhs)
{
    return !lhs == rhs;
}

template<typename Coefficient>
Term& Term<Coefficient>::operator*=(const Coefficient& rhs)
{
    
}
Term& Term<Coefficient>::operator*=(Variable::Arg rhs)
{
    
}
Term& Term<Coefficient>::operator*=(const Term& rhs)
{
    
}

template<typename Coeff>
const Term<Coeff> operator*(const Term<Coeff>& lhs, const Term<Coeff>& rhs);
template<typename Coeff>
const Term<Coeff> operator*(const Term<Coeff>& lhs, const Coeff& rhs);
template<typename Coeff>
const Term<Coeff> operator*(const Coeff& lhs, const Term<Coeff>& rhs);
template<typename Coeff>
const Term<Coeff> operator*(const Term<Coeff>& lhs, Variable::Arg rhs);
template<typename Coeff>
const Term<Coeff> operator*(Variable::Arg, const Term<Coeff>& rhs);
template<typename Coeff>
const Term<Coeff> operator*(const Coeff& lhs, Variable::Arg rhs);
template<typename Coeff>
const Term<Coeff> operator*(Variable::Arg lhs, const Coeff& rhs);
template<typename Coeff>
const Term<Coeff> operator*(const Coeff& lhs, const Monomial& rhs);
template<typename Coeff>
const Term<Coeff> operator*(const Monomial& lhs, const Coeff& rhs);

template<typename Coeff>
std::ostream& operator<<(std::ostream& lhs, const Term<Coeff>& rhs);


}