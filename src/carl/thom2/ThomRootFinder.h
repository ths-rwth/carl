/* 
 * File:   ThomRootFinder.h
 * Author: tobias
 *
 * Created on 19. August 2016, 10:18
 */

#pragma once

#include "../interval/Interval.h"
#include "ThomEncoding.h"
#include "ThomUtil.h"

namespace carl {

template<typename Number>
std::list<ThomEncoding<Number>> realRootsThom(
                const MultivariatePolynomial<Number>& p,
		const std::map<Variable, ThomEncoding<Number>>& m = {},
		const Interval<Number>& interval = Interval<Number>::unboundedInterval()
) {
        CARL_LOG_ASSERT("carl.thom.rootfinder", p.gatherVariables().size() == m.size() + 1, "invalid arguments for realRootsThom");
        using Polynomial = MultivariatePolynomial<Number>;
        std::list<ThomEncoding<Number>> result;
        
        if(p.isUnivariate()) {
                Variable pMainVar = *p.gatherVariables().begin();
                std::list<Polynomial> derivatives = der(p, pMainVar, 1, p.degree(pMainVar));
                
                std::vector<MultivariatePolynomial<Number>> zeroSet = {p};
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
                                pMainVar,
                                nullptr,
                                sd_ptr,
                                1);
                        result.push_back(newEncoding);
                }
                std::cout << "result = " << result << std::endl;
                std::vector<ThomEncoding<Number>> result_vec(result.begin(), result.end());
                std::sort(result_vec.begin(), result_vec.end());
                result = std::list<ThomEncoding<Number>>(result_vec.begin(), result_vec.end());
                return result;
                
                // todo check bounds
        }
        else {
                // some consistency checks...
#ifdef DEVELOPER
                for(const auto& entry : m) {
                        CARL_LOG_ASSERT("carl.thom.rootfinder", entry.first == entry.second.mainVar(), "invalid map Variable -> Thom encoding");
                        CARL_LOG_ASSERT("carl.thom.rootfinder", p.has(entry.first), "");
                }
                
                std::vector<ThomEncoding<Number>> encodings(m.size());
                std::transform(m.begin(), m.end(), encodings.begin(),
                        [](const std::pair<Variable, ThomEncoding<Number>>& entry) {
                                return entry.second();
                        }
                );
                std:::sort(encodings.begin(), encodings.end(),
                        [](const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs) {
                                return lhs.dimension() > rhs.dimension();
                        }
                );
                for(uint i = 0; i < encodings.size() - 1; i++) {
                        CARL_LOG_ASSERT("carl.thom.rootfinder", encodings[i].point() == encodings[i+1], "");
                }
#endif
                // find main var of p
                std::set<Variable> vars = p.gatherVariables();
                for(const auto& entry : m) {
                        vars.erase(entry.first);
                }
                CARL_LOG_ASSERT("carl.thom.rootfinder", vars.size() == 1, "invalid arguments to rootfinder: main var of p could not be determined");
                Variable pMainVar = *vars.begin();
                
                ThomEncoding<Number> point = std::max_element(m.begin(), m.end(),
                        [](const std::pair<Variable, ThomEncoding<Number>>& lhs, const std::pair<Variable, ThomEncoding<Number>>& rhs) {
                                return lhs.second.dimension() < rhs.second.dimension();
                        }
                )->second;
                
                std::list<MultivariatePolynomial<Number>> zeroSet = point.accumulatePolynomials();
                zeroSet.push_front(p);

                
                
                SignDetermination<Number> sd(zeroSet.begin(), zeroSet.end());
                uint numOfRoots = sd.sizeOfZeroSet();
                
                std::list<MultivariatePolynomial<Number>> previousDerivatives = point.sd().processedPolynomials();
                sd.getSignsAndAddAll(previousDerivatives.rbegin(), previousDerivatives.rend());
                
                std::list<MultivariatePolynomial<Number>> pDerivatives = der(p, pMainVar, 1, p.degree(pMainVar));
                
                std::list<SignCondition> signs = {};
                auto it = pDerivatives.rbegin();
                while(signs.size() < numOfRoots) {
                        signs = sd.getSignsAndAdd(*it);
                        it++;
                }
                
                std::shared_ptr<SignDetermination<Number>> sd_ptr = std::make_shared<SignDetermination<Number>>(sd);
                std::shared_ptr<ThomEncoding<Number>> point_ptr = std::make_shared<ThomEncoding<Number>>(point);
                SignCondition pointSigns = point.accumulateSigns();
                for(const auto& sigma : signs) {
                        if(pointSigns.isSuffixOf(sigma)) {
                                SignCondition newSigma(sigma);
                                newSigma.resize(sigma.size() - pointSigns.size());
                                ThomEncoding<Number> newEncoding(
                                        newSigma,
                                        p,
                                        pMainVar,
                                        point_ptr,
                                        sd_ptr,
                                        point.dimension() + 1);
                                result.push_back(newEncoding);
                        }
                }
                return result;
        }
}

} // namespace carl


