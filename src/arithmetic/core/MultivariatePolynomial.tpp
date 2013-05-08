#pragma once
#include "MultivariatePolynomial.h"

namespace arithmetic
{
template<typename Coeff, typename Policy>
Polynomial<Coeff,Policy>::MultivariatePolynomial(Term<Coeff> m) :
mTerms(1,m)
{
    
}
    
template<typename C, typename P>
std::ostream& operator <<( std::ostream& os, const MultivariatePolynomial<C,P>& rhs )
{
    std::copy(rhs.mTerms.begin(), rhs.mTerms.end(),std::ostream_iterator<std::string>(os, ' + '));
    return os;
}
}