/**
 * @file UnivariatePolynomial.h 
 * @author Sebastian Junges
 */

#pragma once

namespace arithmetic
{
template<typename Coefficient>
class UnivariatePolynomial
{
private:
    std::vector<Coefficient> mCoefficients;
};
}