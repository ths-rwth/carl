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
#include "ThomUtil.h"


namespace carl {

/*
 * Returns the roots of the given univariate polynomial encoded in the thom representation.
 * The roots are returned in an ascending order.
 * todo: find roots only in a given intervall
 */
template<typename Coeff>
std::vector<ThomEncoding<Coeff>> realRoots(
        const UnivariatePolynomial<Coeff>& polynomial//,
        //const Interval<Coeff>& interval = Interval<Coeff>::unboundedInterval()       
) {
        assert(!polynomial.isZero());
        std::vector<UnivariatePolynomial<Coeff>> derivatives;
        derivatives.reserve(polynomial.degree() - 1);
        for(uint n = 1; n < polynomial.degree(); n++ ) {
                derivatives.push_back(polynomial.derivative(n));
        }
        assert(derivatives.size() == polynomial.degree() - 1);
        
        // run the core algorithm on p and its derivatives
        std::vector<SignCondition> signConds = signDetermination(derivatives, polynomial, true);
        
        std::vector<ThomEncoding<Coeff>> res;
        res.reserve(signConds.size());
        
        std::shared_ptr<UnivariatePolynomial<Coeff>> ptr = std::make_shared<UnivariatePolynomial<Coeff>>(polynomial);
        
        for(const SignCondition& s : signConds) {
                ThomEncoding<Coeff> t(ptr, s);
                res.push_back(t);
        }
        // sort the roots in an ascending order (using operator <)
        std::sort(res.begin(), res.end());
        
        
        return res;
}
        
} // namespace carl


