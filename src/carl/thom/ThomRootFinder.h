/* 
 * File:   ThomRootFinder.h
 * Author: tobias
 *
 * Created on 3. Mai 2016, 16:06
 */

#pragma once

#include "ThomEncoding.h"
#include "../interval/Interval.h"

namespace carl {

/*
 * 
 */
template<typename Coeff>
std::list<ThomEncoding<Coeff>> realRoots(
        const UnivariatePolynomial<Coeff>& polynomial,
        const Interval<Number>& interval = Interval<Number>::unboundedInterval()       
) {
        std::vector<UnivariatePolynomial<Coeff>> derivatives;
        derivatives.reserve(polynomial.degree() - 1);
        for(uint n = 1; n < polynomial.degree(); n++ ) {
                derivatives.push_back(polynomial.derivative(n));
        }
        assert(derivatives.size() == polynomial.degree() - 1);
        signDetermination(derivatives, p);
        
}
        
} // namespace carl


