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



}