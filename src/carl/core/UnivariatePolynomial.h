/**
 * @file UnivariatePolynomial.h 
 * @author Sebastian Junges
 */

#pragma once

namespace carl
{
template<typename Coefficient>
class UnivariatePolynomial
{
private:
    std::vector<Coefficient> mCoefficients;
};
}