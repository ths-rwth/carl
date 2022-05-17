#pragma once

#include "SqrtEx.h"

namespace carl {

template<typename Poly>
SqrtEx<Poly> substitute( const SqrtEx<Poly>& sqrt_ex, const std::map<Variable, typename SqrtEx<Poly>::Rational>& eval_map ) {
    using Rational = typename SqrtEx<Poly>::Rational;
    Poly radicandEvaluated = carl::substitute(sqrt_ex.radicand(), eval_map );
    Poly factorEvaluated = carl::substitute(sqrt_ex.factor(), eval_map );
    Poly constantPartEvaluated = carl::substitute(sqrt_ex.constantPart(), eval_map );
    Poly denomEvaluated = carl::substitute(sqrt_ex.denominator(), eval_map );
    assert( !denomEvaluated.isConstant() || !carl::is_zero( denomEvaluated.constantPart() ) );
    Rational sqrtExValue;
    if( radicandEvaluated.isConstant() && carl::sqrt_exact( radicandEvaluated.constantPart(), sqrtExValue ) )
    {
        return SqrtEx<Poly>(Poly(constantPartEvaluated + factorEvaluated * sqrtExValue), 
                constant_zero<Poly>::get(), 
                std::move(denomEvaluated), 
                constant_zero<Poly>::get());
    }
    return SqrtEx( std::move(constantPartEvaluated), std::move(factorEvaluated), std::move(denomEvaluated), std::move(radicandEvaluated) );
}

/**
 * Substitutes a variable in an expression by a square root expression, which results in a square root expression.
 * @param _substituteIn The polynomial to substitute in.
 * @param _varToSubstitute The variable to substitute.
 * @param _substituteBy The square root expression by which the variable gets substituted.
 * @return The resulting square root expression.
 */
template<typename Poly>
SqrtEx<Poly> substitute( const Poly& _substituteIn, const carl::Variable _varToSubstitute, const SqrtEx<Poly>& _substituteBy )
{
    if( !_substituteIn.has( _varToSubstitute ) )
        return SqrtEx<Poly>( _substituteIn );
    /*
        * We have to calculate the result of the substitution:
        *
        *                           q+r*sqrt{t}
        *        (a_n*x^n+...+a_0)[------------ / x]
        *                               s
        * being:
        *
        *      sum_{k=0}^n (a_k * (q+r*sqrt{t})^k * s^{n-k})
        *      ----------------------------------------------
        *                           s^n
        */
    auto varInfo = _substituteIn.template getVarInfo<true>( _varToSubstitute );
    const auto& coeffs = varInfo.coeffs();
    // Calculate the s^k:   (0<=k<=n)
    auto coeff = coeffs.begin();
    carl::uint lastDegree = varInfo.maxDegree();
    std::vector<Poly> sk;
    sk.push_back(constant_one<Poly>::get());
    for( carl::uint i = 1; i <= lastDegree; ++i )
    {
        // s^i = s^l * s^{i-l}
        sk.push_back( sk.back() * _substituteBy.denominator() );
    }
    // Calculate the constant part and factor of the square root of (q+r*sqrt{t})^k 
    std::vector<Poly> qk;
    qk.push_back( _substituteBy.constantPart() );
    std::vector<Poly> rk;
    rk.push_back( _substituteBy.factor() );
    // Let (q+r*sqrt{t})^l be (q'+r'*sqrt{t}) 
    // then (q+r*sqrt{t})^l+1  =  (q'+r'*sqrt{t}) * (q+r*sqrt{t})  =  ( q'*q+r'*r't  +  (q'*r+r'*q) * sqrt{t} )
    for( carl::uint i = 1; i < lastDegree; ++i )
    {
        // q'*q+r'*r't
        qk.push_back( qk.back() * _substituteBy.constantPart() + rk.back() * _substituteBy.factor() * _substituteBy.radicand() );
        // q'*r+r'*q
        rk.push_back( rk.back() * _substituteBy.constantPart()  + qk.at( i - 1 ) * _substituteBy.factor() );
    }
    // Calculate the result:
    Poly resFactor = constant_zero<Poly>::get();
    Poly resConstantPart = constant_zero<Poly>::get();
    if( coeff->first == 0 )
    {
        resConstantPart += sk.back() * coeff->second;
        ++coeff;
    }
    for( ; coeff != coeffs.end(); ++coeff )
    {
        resConstantPart += coeff->second * qk.at( coeff->first - 1 ) * sk.at( lastDegree - coeff->first );
        resFactor       += coeff->second * rk.at( coeff->first - 1 ) * sk.at( lastDegree - coeff->first );
    }
    return SqrtEx( resConstantPart, resFactor, sk.back(), _substituteBy.radicand() );
}

}