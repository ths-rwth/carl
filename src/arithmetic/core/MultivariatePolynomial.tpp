#pragma once
#include "Polynomial.h"

namespace arithmetic
{
template<typename Coeff, typename Policy>
Polynomial<Coeff,Policy>::Polynomial(Monomial<Coeff> m) :
mTerms(1,m)
{
    
}
    
template<typename C, typename P>
std::ostream& operator <<( std::ostream& os, const Polynomial<C,P>& rhs )
{
    std::copy(rhs.mTerms.begin(), rhs.mTerms.end(),std::ostream_iterator<std::string>(os, ' + '));
    return os;
}
}