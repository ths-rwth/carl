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

#define THOM_DEBUG
#ifdef THOM_DEBUG
        #define PRINTV(x) std::cout << #x << " = " << x << std::endl;
        #define PRINTV2(x) std::cout << #x << " = " << std::endl << x << std::endl;
        #define PRINT(x) std::cout << x << std::endl;
        #define LINE std::cout << "-----------------------------------------------" << std::endl;
#else
        #define PRINTV(x)
        #define PRINTV2(x)
        #define PRINT
        #define LINE
#endif





namespace carl {
        
// a list of sign conditions that a list of polynomials realizes at a point
// we also view this as a mapping from the polynomials to a Sign
typedef std::vector<Sign> SignCondition; 
        
// todo make this recursive
template<typename Coeff>
std::vector<UnivariatePolynomial<Coeff>> der(const UnivariatePolynomial<Coeff>& p, uint upto = 0) {
        assert(upto <= p.degree());
        if(upto == 0) upto = p.degree();
        std::vector<UnivariatePolynomial<Coeff>> derivatives;
        derivatives.reserve(upto + 1);
        derivatives.push_back(p);
        for(uint n = 1; n <= upto; n++) {
                derivatives.push_back(p.derivative(n));
        }
        return derivatives;
}

template<typename Coeff>
std::vector<MultivariatePolynomial<Coeff>> der(
                const MultivariatePolynomial<Coeff>& p,
                Variable::Arg mainVar,
                uint upto = 0
) {
        size_t d = p.degree(mainVar);
        assert(upto <= d);
        if(upto == 0) upto = d;
        std::vector<MultivariatePolynomial<Coeff>> partialDer;
        partialDer.reserve(upto + 1);
        partialDer.push_back(p);
        for(uint n = 1; n <= upto; n++) {
                partialDer.push_back(partialDer.back().derivative(mainVar));
        }
        return partialDer;
}

// maybe make a class sign condition some day

// states that tau extends sigma (see Algorithms for RAG, p.387)
bool extends(const SignCondition& tau, const SignCondition& sigma) {
        //assert(tau.size() == sigma.size() +1);
        assert(tau.size() > sigma.size());
        for(uint i = tau.size() - sigma.size(); i < tau.size(); i++) {
                if(tau[i] != sigma[i - (tau.size() - sigma.size())]) return false;
        }
        return true;
}

bool isPrefix(const SignCondition& lhs, const SignCondition& rhs) {
        if(lhs.size() > rhs.size()) {
                return false;
        }
        for(uint i = 0; i < lhs.size(); i++) {
                if(lhs[i] != rhs[i]) {
                        return false;
                }
        }
        return true;
}

// compares two sign conditions (associated to the same list of derivatives)
// optimize this so that is returns a comparison result
bool operator<(const SignCondition& lhs, const SignCondition& rhs) {
        assert(lhs.size() == rhs.size());
        assert(lhs.back() == rhs.back());
        if(lhs.size() == 1) return false; // because then they are actually equal
        assert(lhs.size() >= 2);
        for(int i = (int)lhs.size() - 2; i >= 0; i--) {
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