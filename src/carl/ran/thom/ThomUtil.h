/* 
 * File:   ThomUtil.h
 * Author: tobias
 *
 * Created on 19. August 2016, 10:48
 */

#pragma once

#include <carl/poly/umvpoly/MultivariatePolynomial.h>
#include <carl/poly/umvpoly/functions/Derivative.h>

namespace carl {

// list of derivatives p^(from), ..., p^(upto) w.r.t. to var (including both from and upto)
template<typename Number>
std::list<MultivariatePolynomial<Number>> der(const MultivariatePolynomial<Number>& p, Variable::Arg var, uint from, uint upto) {
        assert(upto <= p.degree(var));
        std::list<MultivariatePolynomial<Number>> derivatives;
        if(from > upto) return derivatives; // empty list  
        MultivariatePolynomial<Number> from_th = p;
        for(uint i = 0; i < from; i++) from_th = carl::derivative(from_th, var);
        derivatives.push_back(from_th);
        for(uint n = from + 1; n <= upto; n++) {
                derivatives.push_back(carl::derivative(derivatives.back(), var));
        }
        return derivatives;
}

} // namespace carl

