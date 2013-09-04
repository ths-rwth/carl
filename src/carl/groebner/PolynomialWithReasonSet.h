/* 
 * File:   PolynomialWithReasonSet.h
 * Author: square
 *
 * Created on June 25, 2013, 6:29 PM
 */

#pragma once

#include "../core/MultivariatePolynomial.h"
#include "../core/MultivariatePolynomialPolicy.h"
#include "../util/BitVector.h"

namespace carl
{
template<typename Coeff, typename Ordering = GrLexOrdering, typename Policy=StdMultivariatePolynomialPolicy>
class PolynomialWithReasonSet : public MultivariatePolynomial<Coeff, Ordering, Policy>
{
    BitVector reasonSet;
    public:
	
	PolynomialWithReasonSet() : MultivariatePolynomial<Coeff,Ordering,Policy>()
	{}
	PolynomialWithReasonSet(const MultivariatePolynomial<Coeff,Ordering,Policy>& p) : MultivariatePolynomial<Coeff,Ordering,Policy>(p)
	{}
	
	template<typename InputIterator>
	PolynomialWithReasonSet(InputIterator begin, InputIterator end) : MultivariatePolynomial<Coeff,Ordering, Policy>(begin, end)
	{}
		
		
    const BitVector& getOrigins() const { return reasonSet; }
    void setOrigins(const BitVector& origins) { reasonSet = origins; }
};
}