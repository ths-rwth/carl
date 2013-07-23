/* 
 * File:   PolynomialWithReasonSet.h
 * Author: square
 *
 * Created on June 25, 2013, 6:29 PM
 */

#pragma once

namespace carl
{
template<Coeff, Policy>
PolynomialWithReasonSet : public Polynomial<Coeff, Policy>
{
    BitVector reasonSet;
};
}