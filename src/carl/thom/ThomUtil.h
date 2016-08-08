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
        #define PRINT(x)
        #define LINE
#endif





namespace carl {
        
// a list of sign conditions that a list of polynomials realizes at a point
// we also view this as a mapping from the polynomials to a Sign
typedef std::vector<Sign> SignCondition; 

enum ThomComparisonResult {LESS, EQUAL, GREATER};

        
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

// list of derivatives p^(from), ..., p^(upto) (including both from and upto)
template<typename Coeff>
std::vector<UnivariatePolynomial<Coeff>> der_exp(const UnivariatePolynomial<Coeff>& p, uint from, uint upto) {
        assert(upto <= p.degree());
        std::vector<UnivariatePolynomial<Coeff>> derivatives;
        if(from > upto) return derivatives; // empty list  
        derivatives.reserve(upto - from + 1);
        derivatives.push_back(p.derivative(from));
        for(uint n = from + 1; n <= upto; n++) {
                derivatives.push_back(derivatives.back().derivative());
        }
        return derivatives;
}

// list of derivatives p^(from), ..., p^(upto) (including both from and upto)
template<typename Coeff>
std::vector<MultivariatePolynomial<Coeff>> der_exp(const MultivariatePolynomial<Coeff>& p, Variable::Arg var, uint from, uint upto) {
        assert(upto <= p.degree(var));
        std::vector<MultivariatePolynomial<Coeff>> derivatives;
        if(from > upto) return derivatives; // empty list  
        derivatives.reserve(upto - from + 1);
        MultivariatePolynomial<Coeff> from_th = p;
        for(uint i = 0; i < from; i++) from_th = from_th.derivative(var);
        derivatives.push_back(from_th);
        for(uint n = from + 1; n <= upto; n++) {
                derivatives.push_back(derivatives.back().derivative(var));
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
bool extends(const SignCondition& tau, const SignCondition& sigma);

bool isPrefix(const SignCondition& lhs, const SignCondition& rhs);

// compares two sign conditions (associated to the same list of derivatives)
ThomComparisonResult operator<(const SignCondition& lhs, const SignCondition& rhs);

std::ostream& operator<<(std::ostream& os, const SignCondition& s);

template<typename Number>
Number rumpsBound(const UnivariatePolynomial<Number>& p) {
        assert(!p.isZero());
        uint n = p.degree();
        uint n_half = n % 2 == 0 ? n/2 : (n+1)/2;
        Number norm(0);
        for(const auto& coeff : p.coefficients()) {
                norm += carl::abs(coeff);
        }
        norm += Number(1);
        Number res = Number(14) / Number(5);
        res /= carl::pow(Number(n), n_half + 1);       
        res /= carl::pow(norm, n);
        return res;
}
        
} // namespace carl
