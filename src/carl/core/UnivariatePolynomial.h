/**
 * @file UnivariatePolynomial.h 
 * @author Sebastian Junges
 */

#pragma once

#include "Variable.h"
namespace carl
{
template<typename Coefficient>
class UnivariatePolynomial
{
	public:
	UnivariatePolynomial(Variable::Arg mainVar, std::initializer_list<Coefficient> coefficients);

	template <typename C>
	friend std::ostream& operator<<(std::ostream& os, const UnivariatePolynomial<C>& rhs);

private:
	Variable mMainVar;
    std::vector<Coefficient> mCoefficients;
};
}

#include "UnivariatePolynomial.tpp"