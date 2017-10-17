/** 
 * @file:   MultivariateFactor.tpp
 * @author: Florian Corzilius
 *
 * @since March 10, 2015
 */

#pragma once

namespace carl
{

template<typename C, typename O, typename P>
Factors<MultivariatePolynomial<C,O,P>> MultivariateFactor<C, O, P>::calculate() 
{
	// We start with some trivial cases.
    
    // Calculate the factorization.
    #ifdef USE_GINAC
    return ginacFactorization<Poly>( mp );
    #else 
    if( mp.size() == 1 )
    {
        return factor( mp.lterm() );
    }
    Factors<MultivariatePolynomial<C,O,P>> result;
    result.insert( std::pair<Poly, unsigned>( mp, 1 ) );
    return result;
    #endif

}	

template<typename C, typename O, typename P>
Factors<MultivariatePolynomial<C,O,P>> factor(const Term<C>& _t)
{
	static_assert(is_field<C>::value, "Only implemented for field coefficients");
	if(_t.isConstant())
    {
        Factors<MultivariatePolynomial<C,O,P>> result;
        result.insert( std::pair<MultivariatePolynomial<C,O,P>, unsigned>( MultivariatePolynomial<C,O,P>(_t), 1 ) );
        return result;
    }
    Factors<MultivariatePolynomial<C,O,P>> result = factor(*(_t.monomial()));
    if( !carl::isOne(_t.coeff()) )
    {
        result.insert( std::pair<MultivariatePolynomial<C,O,P>, unsigned>( MultivariatePolynomial<C,O,P>(_t.coeff()), 1 ) );
    }
	return result;
}


template<typename C, typename O, typename P>
Factors<MultivariatePolynomial<C,O,P>> factor(const Monomial::Arg& _m)
{
	Factors<MultivariatePolynomial<C,O,P>> result;
	if (!_m) return result;
	for(const auto& ve : *_m)
	{
		result.insert( std::pair<MultivariatePolynomial<C,O,P>, unsigned>( MultivariatePolynomial<C,O,P>(ve.first), ve.second ) );
	}
	return result;
}

template<typename C, typename O, typename P>
Factors<MultivariatePolynomial<C,O,P>> factor(const MultivariatePolynomial<C,O,P>& _p)
{
	MultivariateFactor<C, O, P> factor_calc(_p);
	return factor_calc.calculate();
}

}
