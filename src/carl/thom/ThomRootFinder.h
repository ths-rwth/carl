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

#include "../core/RealAlgebraicNumber.h"


namespace carl {
        
/*
 * Returns the roots of the given univariate polynomial encoded in the thom representation.
 * The roots are returned in an ascending order.
 */
template<typename Coeff>
std::vector<ThomEncoding<Coeff>> realRoots(
                const UnivariatePolynomial<Coeff>& polynomial,
                const Interval<Coeff>& interval = Interval<Coeff>::unboundedInterval()       
) {
        assert(!polynomial.isZero());
        CARL_LOG_TRACE("carl.thom", "polynomial = " << polynomial << " on " << interval);
        
        std::vector<ThomEncoding<Coeff>> res;
        
        // trivial cases
        
        // polynomial is constant     
        if(polynomial.isConstant() && !polynomial.isZero()) {
                CARL_LOG_INFO("carl.thom", "real roots for non-zero constant polynomial called");
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
        
        // this can be optimized e.g. using binary search ...
        if(interval.lowerBoundType() == BoundType::STRICT) {
                auto it = res.begin();
                while(it != res.end() && *it <= interval.lower()) {
                        it = res.erase(it);
                }
        }
        else if(interval.lowerBoundType() == BoundType::WEAK) {
                auto it = res.begin();
                while(it != res.end() && *it < interval.lower()) {
                        it = res.erase(it);
                }
        }
        
        if(interval.upperBoundType() == BoundType::STRICT) {
                std::reverse(res.begin(), res.end());
                auto it = res.begin();
                while(it != res.end() && *it >= interval.upper()) {
                        it = res.erase(it);;
                }
                std::reverse(res.begin(), res.end());
        }
        else if(interval.upperBoundType() == BoundType::WEAK) {
                std::reverse(res.begin(), res.end());
                auto it = res.begin();
                while(it != res.end() && *it > interval.upper()) {
                        it = res.erase(it);
                }
                std::reverse(res.begin(), res.end());
        }
        
        
        return res;
}

/*
 * finding real roots of a given multivariate polynomial on a point
 */
template<typename Coeff>
std::vector<ThomEncoding<Coeff>> realRoots(
		const MultivariatePolynomial<Coeff>& p,
                Variable::Arg mainVar,
		const ThomEncoding<Coeff>& point,
		const Interval<Coeff>& interval = Interval<Coeff>::unboundedInterval()
) {
        CARL_LOG_TRACE("carl.thom", "p = " << p << " in " << mainVar << ", point = " << point);
        // trivial cases
        
        // main Var does not appear in p should never happen
        CARL_LOG_ASSERT("carl.thom", p.degree(mainVar) > 0, "");
        
        if(p.gatherVariables().size() == 1) {
                CARL_LOG_TRACE("carl.thom", "realRoots for multivariate polynomials with univariate polynomial called")
                assert(p.has(*(p.gatherVariables().begin())));
                return realRoots(p.toUnivariatePolynomial(), interval);
        }
        
        if(point.makesPolynomialZero(p, mainVar)) {
                CARL_LOG_TRACE("carl.thom", "p = " << p << " vanishes on " << point);
                return {ThomEncoding<Coeff>(Coeff(0), mainVar)};
        }
        
        std::vector<MultivariatePolynomial<Coeff>> zeroSet = point.accumulatePolynomials();
        zeroSet.push_back(p);
        if(!isZeroDimensionalSystem(zeroSet)) {
                CARL_LOG_WARN("carl.thom", p << " vanishes on " << point << "... returning 0 as a root");
                return {ThomEncoding<Coeff>(Coeff(0), mainVar)};
        }
        
        std::vector<ThomEncoding<Coeff>> res;
        
        // MAYBE THIS IS WRONG - what if there is no root?
        // p is linear in its main var
        if(p.degree(mainVar) == 1) {
                CARL_LOG_INFO("carl.thom", "real roots for linear polynomial called");
                std::shared_ptr<MultivariatePolynomial<Coeff>> pp = std::make_shared<MultivariatePolynomial<Coeff>>(p);
                std::shared_ptr<ThomEncoding<Coeff>> ppoint = std::make_shared<ThomEncoding<Coeff>>(point);
                ThomEncoding<Coeff> root(pp, mainVar, SignCondition(), ppoint);
                res.push_back(root);
                return res;
        }
        
        
        
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
        
        // this can be optimized e.g. using binary search ...
        if(interval.lowerBoundType() == BoundType::STRICT) {
                auto it = res.begin();
                while(it != res.end() && *it <= interval.lower()) {
                        res.erase(it);
                        it = res.begin();
                }
        }
        else if(interval.lowerBoundType() == BoundType::WEAK) {
                auto it = res.begin();
                while(it != res.end() && *it < interval.lower()) {
                        res.erase(it);
                        it = res.begin();
                }
        }
        
        if(interval.upperBoundType() == BoundType::STRICT) {
                std::reverse(res.begin(), res.end());
                auto it = res.begin();
                while(it != res.end() && *it >= interval.upper()) {
                        res.erase(it);
                        it = res.begin();
                }
                std::reverse(res.begin(), res.end());
        }
        else if(interval.upperBoundType() == BoundType::WEAK) {
                std::reverse(res.begin(), res.end());
                auto it = res.begin();
                while(it != res.end() && *it > interval.upper()) {
                        res.erase(it);
                        it = res.begin();
                }
                std::reverse(res.begin(), res.end());
        }
        
        return res;
}

/*
 * interface intended to be used in the CAD.tpp
 */
template<typename Coeff, typename Number>
std::list<RealAlgebraicNumber<Number>> realRootsThom(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m,
		const Interval<Number>& interval
) {
	CARL_LOG_TRACE("carl.thom", p << " in " << p.mainVar() << ", " << m << ", " << interval);
	assert(m.count(p.mainVar()) == 0);
        assert(!p.isZero());
        
        std::vector<ThomEncoding<Number>> roots;
        
        UnivariatePolynomial<Coeff> tmp(p);
	std::map<Variable, RealAlgebraicNumber<Number>> TEmap;
	
	for (Variable v: tmp.gatherVariables()) {
		if (v == p.mainVar()) continue;
		assert(m.count(v) > 0);
		if (m.at(v).isNumeric()) {
			tmp.substituteIn(v, Coeff(m.at(v).value()));
		} else {
			TEmap.emplace(v, m.at(v));
		}
	}
        //std::cout << "tmp = " << tmp << std::endl;
        //std::cout << "TEmap = " << TEmap << std::endl;
        if(TEmap.empty()) {
                if(tmp.isZero()) return {RealAlgebraicNumber<Number>(0)};
                assert(tmp.gatherVariables().size() == 1);
                 // Coeff = MultivariatePolynomial<Number>, but all coefficients of tmp are numerical
                UnivariatePolynomial<Number> tmp_univ = tmp.convert(std::function<Number(const Coeff&)>([](const Coeff& c){ assert(c.isUnivariate()); return c.constantPart(); }));
                std::list<RealAlgebraicNumber<Number>> roots_triv;
                if(tmp_univ.zeroIsRoot()) {
                        roots_triv.push_back(RealAlgebraicNumber<Number>(0));
                        tmp_univ.eliminateZeroRoots();
                }
                switch (tmp_univ.degree()) {
                        case 0: break;
                        case 1: {
                                Number a = tmp_univ.coefficients()[1], b = tmp_univ.coefficients()[0];
                                assert(a != Number(0));
                                roots_triv.push_back(RealAlgebraicNumber<Number>(-b / a));
                                return roots_triv;
                        }
                        case 2: {
                                Number a = tmp_univ.coefficients()[2], b = tmp_univ.coefficients()[1], c = tmp_univ.coefficients()[0];
                                assert(a != Number(0));
                                /* Use this formulation of p-q-formula:
                                 * x = ( -b +- \sqrt{ b*b - 4*a*c } ) / (2*a)
                                 */
                                Number rad = b*b - 4*a*c;
                                if (rad == 0) {
                                        roots_triv.push_back(RealAlgebraicNumber<Number>(-b / (2*a)));
                                        return roots_triv;
                                }
                                else if (rad > 0) {
                                        std::pair<Number, Number> res = carl::sqrt_fast(rad);
                                        if (res.first == res.second) {
                                                // Root could be calculated exactly
                                                roots_triv.push_back(RealAlgebraicNumber<Number>((-b - res.first) / (2*a)));
                                                roots_triv.push_back(RealAlgebraicNumber<Number>((-b + res.first) / (2*a)));
                                                return roots_triv;
                                        }
                                }
                                break;
                        }
                }
               
                roots = realRoots(tmp_univ, interval);
        }
        else {
        
                MultivariatePolynomial<Number> p_multiv(tmp);
                //std::cout << "p_multiv = " << p_multiv << std::endl;

                if(TEmap.size() == 0) {
                        roots = realRoots(p_multiv.toUnivariatePolynomial(), interval);
                        //std::cout << "roots = \n" << roots << std::endl;
                }

                if(TEmap.size() == 1) {
                        std::pair<Variable, RealAlgebraicNumber<Number>> entry = *TEmap.begin();
                        Variable var = entry.first;
                        RealAlgebraicNumber<Number> ran = entry.second;
                        ThomEncoding<Number> t;
                        if(!ran.isThom()) {
                                assert(ran.isNumeric());
                                t = ThomEncoding<Number>(ran.value(), var);
                        } else {
                                t = ran.getThomEncoding();
                        }
                        //std::cout << " t = " << t << std::endl;
                        roots = realRoots(p_multiv, tmp.mainVar(), t, interval);

                }
                if(TEmap.size() >= 2) assert(false);
        }
        
        CARL_LOG_TRACE("carl.thom", "found the following roots: " << roots);
        
	std::list<RealAlgebraicNumber<Number>> res;
        for(const auto& te : roots) res.push_back(RealAlgebraicNumber<Number>(te));
        return res;
}
        
} // namespace carl


