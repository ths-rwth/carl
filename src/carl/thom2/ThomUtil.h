/* 
 * File:   ThomUtil.h
 * Author: tobias
 *
 * Created on 19. August 2016, 10:48
 */

#pragma once

#include "../core/MultivariatePolynomial.h"

namespace carl {

// list of derivatives p^(from), ..., p^(upto) w.r.t. to var (including both from and upto)
template<typename Number>
std::list<MultivariatePolynomial<Number>> der(const MultivariatePolynomial<Number>& p, Variable::Arg var, uint from, uint upto) {
        assert(upto <= p.degree(var));
        std::list<MultivariatePolynomial<Number>> derivatives;
        if(from > upto) return derivatives; // empty list  
        MultivariatePolynomial<Number> from_th = p;
        for(uint i = 0; i < from; i++) from_th = from_th.derivative(var);
        derivatives.push_back(from_th);
        for(uint n = from + 1; n <= upto; n++) {
                derivatives.push_back(derivatives.back().derivative(var));
        }
        return derivatives;
}

} // namespace carl

