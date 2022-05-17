#pragma once

#include "SqrtEx.h"

namespace carl {

/**
 * @brief Evaluates the square root expression. Might be not exact when a square root is rounded.
 * 
 * @tparam Poly 
 * @param sqrt_ex The square root expression to be evaluated.
 * @param eval_map Assignments for all variables.
 * @param rounding -1 if square root should be rounded downwards, 1 if the square root should be rounded upwards, 0 if double precision is fine.
 * @return std::pair<SqrtEx<Poly>::Rational, bool> The first component is the evaluation result, the second indicates whether the result is exact (true) or rounded (false).
 */
template<typename Poly>
std::pair<typename SqrtEx<Poly>::Rational, bool> evaluate(const SqrtEx<Poly>& sqrt_ex, const std::map<Variable, typename SqrtEx<Poly>::Rational>& eval_map, int rounding) {
    using Rational = typename SqrtEx<Poly>::Rational;

    Poly radicandEvaluated = carl::substitute(sqrt_ex.radicand(), eval_map );
    assert( radicandEvaluated.isConstant() );
    Rational radicandValue = radicandEvaluated.constantPart();
    assert( radicandValue >= 0 );
    Poly factorEvaluated = carl::substitute(sqrt_ex.factor(), eval_map );
    assert( factorEvaluated.isConstant() );
    Rational factorValue = factorEvaluated.constantPart();
    Poly constantPartEvaluated = carl::substitute(sqrt_ex.constantPart(), eval_map );
    assert( constantPartEvaluated.isConstant() );
    Rational constantPartValue = constantPartEvaluated.constantPart();
    Poly denomEvaluated = carl::substitute(sqrt_ex.denominator(), eval_map );
    assert( denomEvaluated.isConstant() );
    Rational denomValue = denomEvaluated.constantPart();
    assert( !carl::is_zero( denomValue ) );
    // Check whether the resulting assignment is integer.
    bool exact = true;
    Rational sqrtExValue;
    if( !carl::sqrt_exact( radicandValue, sqrtExValue ) )
    {
        assert( rounding != 0 );
        exact = false;
        assert( factorValue != 0 );
        double dbSqrt = sqrt( carl::to_double( radicandValue ) );
        sqrtExValue = carl::rationalize<Rational>( dbSqrt ) ;
        // As there is no rational number representing the resulting square root we have to round.
        if( rounding < 0 ) // If the result should round down in this case.
        {
            if( factorValue > 0 && (sqrtExValue*sqrtExValue) > radicandValue )
            {
                // The factor of the resulting square root is positive, hence force rounding down.
                dbSqrt = std::nextafter( dbSqrt, -INFINITY );
                sqrtExValue = carl::rationalize<Rational>( dbSqrt );
                assert( !((sqrtExValue*sqrtExValue) > radicandValue) );
            }
            else if( factorValue < 0 && (sqrtExValue*sqrtExValue) < radicandValue )
            {
                // The factor of the resulting square root is negative, hence force rounding up.
                dbSqrt = std::nextafter( dbSqrt, INFINITY );
                sqrtExValue = carl::rationalize<Rational>( dbSqrt );
                assert( !((sqrtExValue*sqrtExValue) < radicandValue) );
            }
        }
        else if( rounding > 0 ) // If the result should round up in this case.
        {
            if( factorValue < 0 && (sqrtExValue*sqrtExValue) > radicandValue )
            {
                // The factor of the resulting square root is negative, hence force rounding down.
                dbSqrt = std::nextafter( dbSqrt, -INFINITY );
                sqrtExValue = carl::rationalize<Rational>( dbSqrt );
                assert( !((sqrtExValue*sqrtExValue) > radicandValue) );
            }
            else if( factorValue > 0 && (sqrtExValue*sqrtExValue) < radicandValue )
            {
                // The factor of the resulting square root is positive, hence force rounding up.
                dbSqrt = std::nextafter( dbSqrt, INFINITY );
                sqrtExValue = carl::rationalize<Rational>( dbSqrt );
                assert( !((sqrtExValue*sqrtExValue) < radicandValue) );
            }
        }
    }
    return std::make_pair(((constantPartValue + factorValue * sqrtExValue) / denomValue), exact);
}

}