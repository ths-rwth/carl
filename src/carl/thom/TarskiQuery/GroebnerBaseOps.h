/* 
 * File:   GroebnerBaseOps.h
 * Author: tobias
 *
 * Created on 8. Juni 2016, 08:32
 */

#pragma once

#include "../../groebner/groebner.h"
#include "../../core/MultivariatePolynomial.h"

#include <algorithm>


namespace carl {

// use the carl::Term class as a representation for monomials
// this is because a carl::Monomial cannot be 1
template<typename Coeff>
using _Monomial = Term<Coeff>;

// for now ...
template<typename Coeff>
using GB = std::vector<MultivariatePolynomial<Coeff>>;


// cor (the corners of the staircase) are just the leading monomials of the polynomials in the groebner base
template<typename Coeff>
std::vector<_Monomial<Coeff>> cor(const GB<Coeff>& g);


// mon are the monomials below the staircase
// this functions will assert that the result is finite
// todo: currently only works for 2 variables
template<typename Coeff>
std::vector<_Monomial<Coeff>> mon(const GB<Coeff>& g);


// border of staircasie
template<typename Coeff>
std::vector<_Monomial<Coeff>> bor(const GB<Coeff>& g);


// todo
//void corbormon(const Ideal<MultivariatePolynomial<Coeff>>& g, ...)



} // namespace carl

#include "GroebnerBaseOps.tpp"