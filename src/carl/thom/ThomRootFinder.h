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
        
        std::vector<ThomEncoding<Coeff>> res;
        
        // trivial cases
        
        // polynomial is constant     
        if(polynomial.isConstant()) {
                CARL_LOG_INFO("carl.thom", "real roots for constant polynomial called");
                return res;
        }
             
        // polynomial is linear
        if(polynomial.degree() == 1) {
                CARL_LOG_INFO("carl.thom", "real roots for linear polynomial called");
                std::shared_ptr<MultivariatePolynomial<Coeff>> p_ptr = std::make_shared<MultivariatePolynomial<Coeff>>(polynomial);
                ThomEncoding<Coeff> root(p_ptr, SignCondition()); // empty sign condition
                res.push_back(root);
                return res;
        }
        // polynomial is quadratic
        if(polynomial.degree() == 2) {
                // todo implement
        }
        
        std::vector<UnivariatePolynomial<Coeff>> derivatives = der(polynomial, polynomial.degree() - 1);
        // todo make nicer
        derivatives.erase(derivatives.begin());
        assert(derivatives.size() == polynomial.degree() - 1);
        
        // run the sign determination algorithm on the polynomial as zero set and its derivatives
        std::vector<SignCondition> signConds = signDetermination(derivatives, polynomial);
        
        res.reserve(signConds.size());
        
        std::shared_ptr<MultivariatePolynomial<Coeff>> ptr = std::make_shared<MultivariatePolynomial<Coeff>>(polynomial);
        
        for(const SignCondition& s : signConds) {
                ThomEncoding<Coeff> t(ptr, s);
                res.push_back(t);
        }
        // sort the roots in an ascending order (using operator <)
        std::sort(res.begin(), res.end());
        
        
        return res;
}

/*
 * p is a polynomial from a domain like Q[x][y][...
 */
template<typename Coeff>
std::vector<ThomEncoding<Coeff>> realRoots(
		const MultivariatePolynomial<Coeff>& p,
                Variable::Arg mainVar,
		const ThomEncoding<Coeff>& point//,
		//const Interval<Number>& interval = Interval<Number>::unboundedInterval()
) {
        
        // trivial cases
        
        // main Var does not appear in p should never happen
        CARL_LOG_ASSERT("carl.thom", p.degree(mainVar) > 0, "");
        
        std::vector<ThomEncoding<Coeff>> res;
        
        // p is linear in its main var
        if(p.degree(mainVar) == 1) {
                CARL_LOG_INFO("carl.thom", "real roots for linear polynomial called");
                std::shared_ptr<MultivariatePolynomial<Coeff>> pp = std::make_shared<MultivariatePolynomial<Coeff>>(p);
                std::shared_ptr<ThomEncoding<Coeff>> ppoint = std::make_shared<ThomEncoding<Coeff>>(point);
                ThomEncoding<Coeff> root(pp, mainVar, SignCondition(), ppoint);
                res.push_back(root);
                return res;
        }
        
        std::vector<MultivariatePolynomial<Coeff>> zeroSet = point.accumulatePolynomials();
        zeroSet.push_back(p);
        
        // maybe these derivatives should rather be a list...
        std::vector<MultivariatePolynomial<Coeff>> derivatives;
        if(p.degree(mainVar) > 1) {
                derivatives = der(p, mainVar, p.degree(mainVar) - 1);
                derivatives.erase(derivatives.begin());
        }
        else {
                derivatives = std::vector<MultivariatePolynomial<Coeff>>(); // empty
        }
        std::vector<MultivariatePolynomial<Coeff>> pointDer = point.accumulateDer();
        derivatives.insert(derivatives.begin(), pointDer.begin(), pointDer.end());
        
        // we are ready for sign determination!
        std::vector<SignCondition> signConds = signDetermination(derivatives, zeroSet);
        
        // find the sign conditions in signCond with the prefix prevSignCond
        // then cut off this prefix from the resulting sign conds
        std::vector<SignCondition> newConds;
        SignCondition prevSignCond = point.accumulateSigns();
        for(SignCondition& s : signConds) {
                if(isPrefix(prevSignCond, s)) {
                        assert(prevSignCond.size() <= s.size());
                        auto it = s.begin() + prevSignCond.size();
                        s.erase(s.begin(), it);
                        newConds.push_back(s);
                }
        }
        std::cout << "newConds" << newConds << std::endl;
        
        
        // construct the thom encodings from p, this new sign conditions and point
        std::shared_ptr<MultivariatePolynomial<Coeff>> pp = std::make_shared<MultivariatePolynomial<Coeff>>(p);
        std::shared_ptr<ThomEncoding<Coeff>> ppoint = std::make_shared<ThomEncoding<Coeff>>(point);
        for(const auto& s : newConds) {
                ThomEncoding<Coeff> enc(pp, mainVar, s, ppoint);
                assert(!enc.isOneDimensional());
                res.push_back(enc);
        }
        
        std::sort(res.begin(), res.end());
        return res;
}
        
} // namespace carl


