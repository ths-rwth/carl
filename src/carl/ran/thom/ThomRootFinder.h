/* 
 * File:   ThomRootFinder.h
 * Author: tobias
 *
 * Created on 19. August 2016, 10:18
 */

#pragma once

#include <carl/interval/Interval.h>
#include "ThomEncoding.h"
#include "ThomUtil.h"
#include "ran_thom.h"

namespace carl {
        
// forward declarations
template<typename Number>
class ThomEncoding;
template<typename Number>
class RealAlgebraicNumber;
  

template<typename Number>
std::list<ThomEncoding<Number>> realRootsThom(
                const MultivariatePolynomial<Number>& p,
                Variable::Arg mainVar,
		std::shared_ptr<ThomEncoding<Number>> point_ptr,
		const Interval<Number>& interval = Interval<Number>::unbounded_interval()
);
        
/*
 * Analyzes p and m in order to find a (recursive) Thom encoding on which p can be lifted
 */
template<typename Number>
std::list<ThomEncoding<Number>> realRootsThom(
                const MultivariatePolynomial<Number>& p,
                Variable::Arg mainVar,
		const std::map<Variable, ThomEncoding<Number>>& m = {},
		const Interval<Number>& interval = Interval<Number>::unbounded_interval()
) {
        CARL_LOG_INFO("carl.thom.rootfinder",
                "\n---------------------------------------------\n"
                << "Thom Root Finder called:\n"
                << "---------------------------------------------\n"
                << "p = " << p << " in " << mainVar << "\n"
                << "m = " << m << "\n"
                << "interval = " << interval << "\n"
                << "---------------------------------------------");
        CARL_LOG_ASSERT("carl.thom.rootfinder", !p.is_constant(), "this should not be handled here but somewhere before");
        //CARL_LOG_ASSERT("carl.thom.rootfinder", p.has(mainVar), "");
        // attention ...
        if(!p.has(mainVar)) {
                return std::list<ThomEncoding<Number>>();
        }
        
        for(const auto& entry : m) {
                CARL_LOG_ASSERT("carl.thom.rootfinder", entry.first == entry.second.mainVar(), "invalid map Variable -> Thom encoding");
        }
        for(const auto& v : carl::variables(p)) {
                if(v != mainVar) {
                        CARL_LOG_ASSERT("carl.thom.rootfinder", m.find(v) != m.end(), "there is a variable which is in p but not in m");
                }
        }
        
        if(p.is_univariate()) {
                return realRootsThom<Number>(p, mainVar, nullptr, interval);
        }
        CARL_LOG_ASSERT("carl.thom.rootfinder", m.size() > 0, "");
        CARL_LOG_ASSERT("carl.thom.rootfinder", m.size() == carl::variables(p).size() - 1, "");
        
        ThomEncoding<Number> point = ThomEncoding<Number>::analyzeTEMap(m);
        std::shared_ptr<ThomEncoding<Number>> point_ptr = std::make_shared<ThomEncoding<Number>>(point);
        return realRootsThom(p, mainVar, point_ptr, interval);
        
        /* In m, there is either one descending chain and a dimension-1 encoding or just one descending chain (but not quite sure about this) */
        
//        ThomEncoding<Number> point = std::max_element(m.begin(), m.end(),
//                        [](const std::pair<Variable, ThomEncoding<Number>>& lhs, const std::pair<Variable, ThomEncoding<Number>>& rhs) {
//                                return lhs.second.dimension() < rhs.second.dimension();
//                        }
//        )->second;
//        CARL_LOG_TRACE("carl.thom.rootfinder", "maximal encoding in m w.r.t. to dimension: " << point);
//        
//        // TODO assert that the encodings in the chain are actually the encodings in the map
//        
//        if(point.dimension() >= m.size()) {
//                // in this case there is just one descending chain
//                CARL_LOG_TRACE("carl.thom.rootfinder", "found single descending chain in m: " << point);
//                std::shared_ptr<ThomEncoding<Number>> point_ptr = std::make_shared<ThomEncoding<Number>>(point);
//                return realRootsThom(p, mainVar, point_ptr, interval);
//        }
//        
//        else{
//                for(const auto& entry : m) {
//                        CARL_LOG_ASSERT("carl.thom.rootfinder", entry.second.dimension() == 1, "this is an assumption i have made");
//                }
//                auto m_it = m.begin();
//                point = m_it->second;
//                m_it++;
//                std::shared_ptr<ThomEncoding<Number>> point_ptr = std::make_shared<ThomEncoding<Number>>(point);
//                while(m_it != m.end()) {
//                        ThomEncoding<Number> newPoint(m_it->second, point_ptr);
//                        point = newPoint;
//                        point_ptr = std::make_shared<ThomEncoding<Number>>(point);
//                        m_it++;
//                }
//                CARL_LOG_TRACE("carl.thom.rootfinder", "point = " << point);
//                return realRootsThom(p, mainVar, point_ptr, interval);
//        }
}

/*
 * don't call this directly
 */
template<typename Number>
std::list<ThomEncoding<Number>> realRootsThom(
                const MultivariatePolynomial<Number>& p,
                Variable::Arg mainVar,
		std::shared_ptr<ThomEncoding<Number>> point_ptr,
		const Interval<Number>& interval
) {
        using Polynomial = MultivariatePolynomial<Number>;
        std::list<ThomEncoding<Number>> result;
        
        
        
        if(point_ptr == nullptr) {
                std::list<Polynomial> derivatives = der(p, mainVar, 1, p.degree(mainVar));
                
                std::vector<Polynomial> zeroSet = {p};
                SignDetermination<Number> sd(zeroSet.begin(), zeroSet.end());
                
                uint numOfRoots = sd.sizeOfZeroSet();
                if(numOfRoots == 0) return {};
                
                std::list<SignCondition> signs = {};
                auto it = derivatives.rbegin();
                while(signs.size() < numOfRoots) {
                        signs = sd.getSignsAndAdd(*it);
                        it++;
                }
                std::shared_ptr<SignDetermination<Number>> sd_ptr = std::make_shared<SignDetermination<Number>>(sd);
                for(const auto& sigma : signs) {
                        ThomEncoding<Number> newEncoding(
                                sigma,
                                p,
                                mainVar,
                                nullptr,
                                sd_ptr,
                                sigma.size());
                        result.push_back(newEncoding);
                }
            
        }
        else {
                // check if p vanishes on point
                if(point_ptr->makesPolynomialZero(p, mainVar)) {
                        return {ThomEncoding<Number>(Number(0), mainVar)};
                }
                
                std::list<Polynomial> zeroSet = point_ptr->accumulatePolynomials();
                zeroSet.push_front(p);

                SignDetermination<Number> sd(zeroSet.begin(), zeroSet.end());
                uint numOfRoots = sd.sizeOfZeroSet();
                if(numOfRoots == 0) return {};
                
                std::list<Polynomial> pointDerivatives = point_ptr->sd().processedPolynomials();
                sd.getSignsAndAddAll(pointDerivatives.rbegin(), pointDerivatives.rend());
                
                std::list<Polynomial> pDerivatives = der(p, mainVar, 1, p.degree(mainVar));
                
                std::list<SignCondition> signs = {};
                uint relevant = 0;
                auto it = pDerivatives.rbegin();
                while(signs.size() < numOfRoots) {
                        signs = sd.getSignsAndAdd(*it);
                        it++;
                        relevant++;
                }
                
                std::shared_ptr<SignDetermination<Number>> sd_ptr = std::make_shared<SignDetermination<Number>>(sd);
                SignCondition pointSigns = point_ptr->accumulateRelevantSigns();
                for(const auto& sigma : signs) {
                        CARL_LOG_ASSERT("carl.thom.rootfinder", sigma.size() == pointSigns.size() + relevant, "");
                        if(pointSigns.isSuffixOf(sigma)) {
                                SignCondition newSigma(sigma);
                                newSigma.resize(relevant);
                                ThomEncoding<Number> newEncoding(
                                        newSigma,
                                        p,
                                        mainVar,
                                        point_ptr,
                                        sd_ptr,
                                        relevant
                                );
                                result.push_back(newEncoding);
                        }
                }
        }
        
        // convert in a vector because std::sort needs random access iterator
        std::vector<ThomEncoding<Number>> result_vec(result.begin(), result.end());
        std::sort(result_vec.begin(), result_vec.end());
        result = std::list<ThomEncoding<Number>>(result_vec.begin(), result_vec.end());
        
        // check bounds
        // this could be optimized e.g. using binary search ...
        if(interval.lower_bound_type() == BoundType::STRICT) {
                auto it = result.begin();
                while(it != result.end() && *it <= interval.lower()) {
                        it = result.erase(it);
                }
        }
        else if(interval.lower_bound_type() == BoundType::WEAK) {
                auto it = result.begin();
                while(it != result.end() && *it < interval.lower()) {
                        it = result.erase(it);
                }
        }
        
        if(interval.upper_bound_type() == BoundType::STRICT) {
                std::reverse(result.begin(), result.end());
                auto it = result.begin();
                while(it != result.end() && *it >= interval.upper()) {
                        it = result.erase(it);;
                }
                std::reverse(result.begin(), result.end());
        }
        else if(interval.upper_bound_type() == BoundType::WEAK) {
                std::reverse(result.begin(), result.end());
                auto it = result.begin();
                while(it != result.end() && *it > interval.upper()) {
                        it = result.erase(it);
                }
                std::reverse(result.begin(), result.end());
        }
        
        CARL_LOG_INFO("carl.thom.rootfinder", "found the following roots: " << result);
        return result;
}

/*
 * Interface to the Thom mechanism used in the CAD.
 * 
 * Input:
 *      A (possibly multivariate) polynomial p represented with respect
 *      to the variable of the current level.
 *      A mapping m from a set of variables to a set of RANs that are either
 *      represented explicitly or as Thom encodings. All variables of p that
 *      are not the main variable must appear in m.
 *      An interval where we are looking for roots.
 * 
 * Output:
 *      The list of RANs representing the real roots of the univariate polynomial
 *      obtained by substituting the coeffcients of p according to m.
 *      
 * Overview:
 *      1. Plug in all RANs in m which are explicitly represented into p
 *      2. If the resulting polynomial has at most degree 2, try to solve trivial
 *      3. Use the remaining Thom encodings in m to construct a real alg. point
 *         represented as a Thom encoding
 *      4. Determine roots according to this point
 */
        

template<typename Coeff, typename Number>
std::list<RealAlgebraicNumber<Number>> realRootsThom(
		const UnivariatePolynomial<Coeff>& p,
		const std::map<Variable, RealAlgebraicNumber<Number>>& m,
		const Interval<Number>& interval
) {
        CARL_LOG_TRACE("carl.thom.rootfinder", p << " in " << p.mainVar() << ", " << m << ", " << interval);
	assert(m.count(p.mainVar()) == 0);
        assert(!carl::is_zero(p));
        
        std::list<ThomEncoding<Number>> roots;
        
        UnivariatePolynomial<Coeff> tmp(p);
	std::map<Variable, ThomEncoding<Number>> TEmap;
	
	for (Variable v: variables(tmp)) {
		if (v == p.mainVar()) continue;
		assert(m.count(v) > 0);
		if (m.at(v).is_numeric()) {
			substitute_inplace(tmp, v, Coeff(m.at(v).value()));
		} else {
			TEmap.emplace(v, m.at(v).getThomEncoding());
		}
	}
        CARL_LOG_TRACE("carl.thom.rootfinder", "TEmap = " << TEmap);
        CARL_LOG_TRACE("carl.thom.rootfinder", "tmp = " << tmp);
        std::list<RealAlgebraicNumber<Number>> roots_triv;
        if(variables(tmp).size() <= 1) {
                if(carl::is_zero(tmp)) return {RealAlgebraicNumber<Number>(0)};
                assert(variables(tmp).size() == 1);
                 // Coeff = MultivariatePolynomial<Number>, but all coefficients of tmp are numerical
                UnivariatePolynomial<Number> tmp_univ = tmp.convert(std::function<Number(const Coeff&)>([](const Coeff& c){ assert(c.is_univariate()); return c.constant_part(); }));
                if(tmp_univ.zero_is_root()) {
                        roots_triv.push_back(RealAlgebraicNumber<Number>(0));
                        tmp_univ.eliminateZeroRoots();
                }
                CARL_LOG_TRACE("carl.thom.rootfinder", "tmp_univ = " << tmp_univ);
                switch (tmp_univ.degree()) {
                        case 0: {
                                CARL_LOG_TRACE("carl.thom.rootfinder", "roots_triv = " << roots_triv);
                                return roots_triv;
                        }
                        case 1: {
                                Number a = tmp_univ.coefficients()[1], b = tmp_univ.coefficients()[0];
                                assert(a != Number(0));
                                roots_triv.push_back(RealAlgebraicNumber<Number>(-b / a));
                                CARL_LOG_TRACE("carl.thom.rootfinder", "roots_triv = " << roots_triv);
                                return roots_triv;
                        }
                        case 2: {
                                Number a = tmp_univ.coefficients()[2], b = tmp_univ.coefficients()[1], c = tmp_univ.coefficients()[0];
                                assert(a != Number(0));
                                CARL_LOG_TRACE("carl.thom.rootfinder", "a = " << a << ", b = " << b << ", c = " << c);
                                /* Use this formulation of p-q-formula:
                                 * x = ( -b +- \sqrt{ b*b - 4*a*c } ) / (2*a)
                                 */
                                Number rad = b*b - 4*a*c;
                                if (rad == 0) {
                                        roots_triv.push_back(RealAlgebraicNumber<Number>(-b / (2*a)));
                                        CARL_LOG_TRACE("carl.thom.rootfinder", "roots_triv = " << roots_triv);
                                        return roots_triv;
                                }
                                /*
                                else if (rad > 0) {
                                        std::pair<Number, Number> res = carl::sqrt_fast(rad);
                                        CARL_LOG_TRACE("carl.thom.rootfinder", "carl::sqrt_fast(rad) returned " << res);
                                        if (res.first == res.second) {
                                                // Root could be calculated exactly
                                                roots_triv.push_back(RealAlgebraicNumber<Number>((-b - res.first) / (2*a)));
                                                roots_triv.push_back(RealAlgebraicNumber<Number>((-b + res.first) / (2*a)));
                                                CARL_LOG_TRACE("carl.thom.rootfinder", "roots_triv = " << roots_triv);
                                                return roots_triv;
                                        }
                                }
                                */
                                break;
                        }
                }
               
                roots = realRootsThom<Number>(MultivariatePolynomial<Number>(tmp_univ), tmp_univ.mainVar(), nullptr, interval);
                
        }
        else {
                // need to perform 'real' lifting
                
                MultivariatePolynomial<Number> p_multiv(tmp);
                //std::cout << "p_multiv = " << p_multiv << std::endl;

                roots = realRootsThom(p_multiv, p.mainVar(), TEmap, interval);
        }
        
        
        
	std::list<RealAlgebraicNumber<Number>> res;
        for(const auto& te : roots) res.push_back(RealAlgebraicNumber<Number>(te));
        for(const auto& number : roots_triv) res.push_back(number);
        
        CARL_LOG_TRACE("carl.thom.rootfinder", "found the following roots: " << res);
        
        return res;
}


        
} // namespace carl
