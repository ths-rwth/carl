#pragma once
#include "MultivariatePolynomial.h"
#include "logging.h"

namespace arithmetic
{
template<typename Coeff, typename Policy>
MultivariatePolynomial<Coeff,Policy>::MultivariatePolynomial(const Term<Coeff>& t) :
mTerms(1,t)
{
    
}
    
template<typename Coeff, typename Policy>
std::shared_ptr<const Monomial> MultivariatePolynomial<Coeff,Policy>::lmon() const
{
    return lterm()->monomial();
}
template<typename Coeff, typename Policy>
std::shared_ptr<const Term<Coeff>> MultivariatePolynomial<Coeff,Policy>::lterm() const
{
    LOG_ASSERT(!isZero, "Leading term undefined on zero polynomials.");
    if(Policy::Ordering::degreeOrdering)
    {
        return mTerms.front();
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
    }
}
template<typename Coeff, typename Policy>
Coeff MultivariatePolynomial<Coeff,Policy>::lcoeff() const
{
    return lterm()->coeff();
}

template<typename Coeff, typename Policy>
exponent MultivariatePolynomial<Coeff,Policy>::highestDegree() const
{
    if(mTerms.size == 0) return 0;
    if(Policy::Ordering::degreeOrdering)
    {
        return mTerms.front()->tdeg();
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
    }
}

template<typename Coeff, typename Policy>
bool MultivariatePolynomial<Coeff,Policy>::isZero() const
{
    return mTerms.empty();
}
template<typename Coeff, typename Policy>
bool MultivariatePolynomial<Coeff,Policy>::isConstant() const
{
    return (mTerms.size == 0) || (mTerms.size == 1 && mTerms.front()->isConstant());
}
template<typename Coeff, typename Policy>
bool MultivariatePolynomial<Coeff,Policy>::isLinear() const
{
    if(mTerms.size == 0) return true;
    if(Policy::Ordering::degreeOrdering)
    {
        return mTerms.front()->isLinear();
    }
    else 
    {
        LOG_NOTIMPLEMENTED();
    }
}

template<typename Coeff, typename Policy>
std::shared_ptr<const Term<Coeff>> MultivariatePolynomial<Coeff,Policy>::constantPart() const
{
    LOG_NOTIMPLEMENTED();
}

template<typename Coeff, typename Policy>
bool MultivariatePolynomial<Coeff,Policy>::isTsos() const
{
    LOG_NOTIMPLEMENTED();
    return false;
}

template<typename C, typename P>
std::ostream& operator <<( std::ostream& os, const MultivariatePolynomial<C,P>& rhs )
{
    std::copy(rhs.mTerms.begin(), rhs.mTerms.end(),std::ostream_iterator<std::string>(os, " + "));
    return os;
}
}