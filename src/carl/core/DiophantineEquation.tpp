/**
 * @file DiophantineEquation.tpp
 * @author Tobias Winkler
 * 
 */

#pragma once

#include "DiophantineEquation.h"

namespace carl {

template<typename T>
std::vector<T> solveDiophantine(MultivariatePolynomial<T>& p) {
	static_assert(carl::is_integer<T>::value, "Diophantine equations are build from integral coefficients only!");
	std::vector<T> res;
        res = std::vector<T>();
	
        // use an algorithm for linear equations only
        if(p.isLinear()) {
            res = solveLinearDiophantine(p);  
        }
        else {
            CARL_LOG_NOTIMPLEMENTED();
        }
            
	return res;
}

// Finds a single solution of a linear diophantine Equation
template<typename T>
std::vector<T> solveLinearDiophantine(MultivariatePolynomial<T>& equation) {
    assert(equation.isLinear());
    //assert(!equation.isConstant());
    //assert(equation.hatConstantTerm()); // change this later
    
    std::vector<T> res;
    
    // if there is no constant part return the trivial solution
    if(!equation.hasConstantTerm()){
        res = std::vector<T>(equation.gatherVariables().size(), 0);
        return res;
    }
    
    T const_part = equation.constantPart();
    
    // initialize coefficient vector
    std::vector<carl::Term<T>> terms = equation.getTerms();
    std::vector<T> coeffs = std::vector<T>();
    for(std::size_t i = 1; i < terms.size(); i++ ) { // terms[0] is the constant part
        coeffs.push_back(terms[i].coeff());
    }
    
    
    std::vector<T> fromExtendedGcd = std::vector<T>(1, 1);
    T currGcd = coeffs[0];
    
    if(carl::mod(const_part, coeffs[0]) == 0) {
        res = std::vector<T>(coeffs.size(), 0);
        res[0] = -carl::div(const_part, coeffs[0]);
        return res;
    }
    for(std::size_t i = 1; i < coeffs.size(); i++) {
        T s;
        T t;
        currGcd = extended_gcd_integer(currGcd, coeffs[i], s, t);
        for(auto& r : fromExtendedGcd) {
            r *= s;
        }
        fromExtendedGcd.push_back(t);
        if(carl::mod(const_part, currGcd) == 0) {
            T factor = -carl::div(const_part, currGcd);
            for(auto& r : fromExtendedGcd) {
                r *= factor;
            }
            std::size_t diff = coeffs.size() - fromExtendedGcd.size();
            for(std::size_t j = 0; j < diff; j++) {
                fromExtendedGcd.push_back(0);
            }
            assert(fromExtendedGcd.size() == coeffs.size());
            return fromExtendedGcd;
        }
    }
    // no solution exists
    CARL_LOG_NOTIMPLEMENTED(); 
    return std::vector<T>(coeffs.size(), 0);
    
}

// implementation of extended euklid for integers
template<typename T>
T extended_gcd_integer(T a, T b, T& s, T& t) {
    static_assert(carl::is_integer<T>::value, "extended gcd algorithmn is for integral types only!");
    
    // gcd(0, 0) := 0
    if(a == 0 && b == 0) {
        s = 0; t = 0;
        return 0;
    }
    
    s = 0;
    T old_s = 1;
    t = 1;
    T old_t = 0;
    T r = b;
    T old_r = a;
    T quotient;
    
    T temp;
    
    while(r != 0) {
        quotient = carl::quotient(old_r, r);
        
        temp = r;
        r = old_r - quotient * r;
        old_r = temp;
        
        temp = s;
        s = old_s - quotient * s;
        old_s = temp;
        
        temp = t;
        t = old_t - quotient * t;
        old_t = temp;
    }
    // normalize so that the gcd is always positive
    if(old_r < 0) {
        old_s = -old_s;
        old_t = -old_t;
        old_r = -old_r;
    }
    s = old_s;
    t = old_t;
    return old_r;
}

}
