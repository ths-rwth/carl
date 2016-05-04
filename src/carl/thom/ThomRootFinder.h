/* 
 * File:   ThomRootFinder.h
 * Author: tobias
 *
 * Created on 3. Mai 2016, 16:06
 */

#pragma once

#include <algorithm>

#include "ThomEncoding.h"
#include "../interval/Interval.h"


namespace carl {

/*
 * 
 */
template<typename Coeff>
std::vector<ThomEncoding<Coeff>> realRoots(
        const UnivariatePolynomial<Coeff>& polynomial,
        const Interval<Coeff>& interval = Interval<Coeff>::unboundedInterval()       
) {
        assert(!polynomial.isZero());
        std::vector<UnivariatePolynomial<Coeff>> derivatives;
        derivatives.reserve(polynomial.degree() - 1);
        for(uint n = 1; n < polynomial.degree(); n++ ) {
                derivatives.push_back(polynomial.derivative(n));
        }
        assert(derivatives.size() == polynomial.degree() - 1);
        
        // run the core algorithm on p and its derivatives
        std::vector<SignCondition> signConds = signDetermination(derivatives, polynomial);
        
        std::vector<ThomEncoding<Coeff>> res;
        res.reserve(signConds.size());
        
        // this pointer cant be a local variable??
        std::shared_ptr<UnivariatePolynomial<Coeff>>* ptr = new std::shared_ptr<UnivariatePolynomial<Coeff>>();
        *ptr = std::make_shared<UnivariatePolynomial<Coeff>>(polynomial);
        
        for(const SignCondition& s : signConds) {
                ThomEncoding<Coeff> t(*ptr, s);
                res.push_back(t);
        }
        std::sort(res.begin(), res.end());
        
        std::cout << "Root finder result: " << res << std::endl;
        
        return res;
}
        
} // namespace carl


