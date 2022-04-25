#pragma once

#include "FactorizedPolynomial.h"
#include <carl/core/polynomialfunctions/IntervalEvaluation.h>

namespace carl {

/**
 * Like substitute, but expects substitutions for all variables.
 * @return For a polynomial p, the function value p(x_1,...,x_n).
 */
template<typename Coeff, typename Subst>
Subst evaluate(const FactorizedPolynomial<Coeff>& p, const std::map<Variable, Subst>& substitutions) {
	if (!existsFactorization(p)) {
		return p.coefficient();
	}
	if (p.factorizedTrivially()) {
		return Subst(p.coefficient()) * carl::evaluate(p.polynomial(), substitutions);
	} else {
		Subst result = p.coefficient();
		for (const auto& factor : p.factorization()) {
			Subst subResult = carl::evaluate(factor.first, substitutions);
			if (carl::isZero(subResult)) {
				return constant_zero<Subst>::get();
			}
			result *= carl::pow(subResult, factor.second);
		}
		assert(result == carl::evaluate(computePolynomial(p), substitutions));
		return result;
	}
}

template<typename P, typename Numeric>
Interval<Numeric> evaluate(const FactorizedPolynomial<P>& p, const std::map<Variable, Interval<Numeric>>& map)
{
	if( !existsFactorization( p ) )
		return Interval<Numeric>( p.coefficient() );
	if( p.factorizedTrivially() )
	{
		return evaluate( p.polynomial(), map ) * Interval<Numeric>( p.coefficient() );
	}
	else
	{
		Interval<Numeric> result( p.coefficient() );
		for( const auto& factor : p.factorization() )
		{
			Interval<Numeric> factorEvaluated = evaluate( factor.first, map );
			if( factorEvaluated.isZero() )
				return factorEvaluated;
			result *= factorEvaluated.pow( factor.second );
		}
		return std::move( result );
	}
}

}