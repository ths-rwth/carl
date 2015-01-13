/**
 * @file DiophantineEquations.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include "../numbers/numbers.h"
#include "MultivariatePolynomial.h"

namespace carl {


    
template<typename T>
T gcdOfLinearCoefficients(MultivariatePolynomial<T>& p) {
    std::vector<carl::Term<T>> terms = p.getTerms();
    T res = 0;
    T& currGcd = res;
    // the first element in terms is the constant part, if there is any
    int i = p.hasConstantTerm() ? 1 : 0;
    for(; i < terms.size(); i++) {
        currGcd = gcd_assign(currGcd, terms[i].coeff()); // warum funktioniert das?
    }
    return res;
} 

/**
 * Diophantine Equations solver
 */
template<typename T>
std::vector<T> solveDiophantine(MultivariatePolynomial<T>& p) {
	static_assert(carl::is_integer<T>::value, "Diophantine equations are build from integral coefficients only!");
	std::vector<T> res;
	
        // use an algorithm for linear equations only
        if(p.isLinear()) {
            if(carl::mod(p.constantPart(), carl::gcdOfLinearCoefficients(p)) != 0) {
                // in this case no solutions exist
                // todo: what is the return value if there are no solutions?
            }
            else {
                // find the smalltest coeff
                int coeffBeginPos = p.hasConstantTerm() ? 1 : 0;
                std::vector<carl::Term<T>>& terms = p.getTerms();
                T min = carl::abs(terms[coeffBeginPos].coeff());
                for(int i = coeffBeginPos + 1; i < terms.size(); i++) {
                    if(carl::abs(terms[i].coeff()) < min) {
                        min = terms[i].coeff();
                    }
                }
                
                
            }
        }
            
	return res;
}




}