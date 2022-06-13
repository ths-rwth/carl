/**
 * @file DiophantineEquation.h
 * @author Tobias Winkler <tobias.winkler1@rwth-aachen.de>
 */

#pragma once

#include <carl-arith/numbers/numbers.h>
#include "../MultivariatePolynomial.h"

namespace carl {

    
/**
 * Diophantine Equations solver
 */
template<typename T>
std::vector<T> solveDiophantine(MultivariatePolynomial<T> & p);

template<typename T>
T extended_gcd_integer(T a, T b, T& s, T& t);



} // namespace carl

#include "DiophantineEquation.tpp"