/* 
 * File:   ThomUtil.h
 * Author: tobias
 *
 * Created on 6. Mai 2016, 10:17
 */

#pragma once

#include <vector>

#include "../core/UnivariatePolynomial.h"
#include "../core/Sign.h"

typedef std::vector<Sign> SignCondition; 


namespace carl {
        
        // todo make this recursive
        template<typename Coeff>
        std::vector<UnivariatePolynomial<Coeff>> der(const UnivariatePolynomial<Coeff>& p) {
                std::vector<UnivariatePolynomial<Coeff>> derivatives;
                derivatives.reserve(p.degree() + 1);
                derivatives.push_back(p);
                for(uint n = 1; n <= p.degree(); n++) {
                        derivatives.push_back(p.derivative(n));
                }
                return derivatives;
        }
        
        // maybe make a class sign condition some day
        
        // states that tau extends sigma (see Algorithms for RAG, p.387)
        bool extends(const SignCondition& tau, const SignCondition& sigma) {
                //assert(tau.size() == sigma.size() +1);
                assert(tau.size() > sigma.size());
                for(int i = tau.size() - sigma.size(); i < tau.size(); i++) {
                        if(tau[i] != sigma[i - (tau.size() - sigma.size())]) return false;
                }
                //PRINT(tau << " extends " << sigma);
                return true;
        }
        
        // compares two sign conditions (associated to the same list of derivatives)
        bool operator<(const SignCondition& lhs, const SignCondition& rhs) {
                assert(lhs.size() == rhs.size());
                assert(lhs.back() == rhs.back());
                if(lhs.size() == 1) return false; // because then they are actually equal
                assert(lhs.size() >= 2);
                int n = lhs.size();
                for(int i = n - 2; i >= 0; i--) {
                        if(lhs[i] != rhs[i]) {
                                assert(lhs[i+1] == rhs[i+1]);
                                if(lhs[i+1] == Sign::POSITIVE) {
                                        return lhs[i] < rhs[i];
                                }
                                else {
                                        return lhs[i] > rhs[i];
                                }
                        }
                }
                return false;
        }
        
} // namespace carl