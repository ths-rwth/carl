/* 
 * File:   ThomEncoding.h
 * Author: tobias
 *
 * Created on 24. April 2016, 20:04
 */

#pragma once


#include "../core/UnivariatePolynomial.h"
#include "../core/Sign.h"

namespace carl {


typedef std::vector<Sign> SignCondition; // a list of sign conditions that a list of polynomials realizes at a point
                                         // we also view this as a mapping from the polynomials to a Sign

/*
 * Calculates the set of sign conditions realized by the polynomials in the list p on the roots of z.
 */
template<typename Coeff>
std::vector<SignCondition> signDetermination(const std::vector<UnivariatePolynomial<Coeff>>& p, const UnivariatePolynomial<Coeff>& z);


#include "ThomEncoding.tpp"
} // namespace carl




