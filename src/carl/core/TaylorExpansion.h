/* 
 * File:   TaylorExpansion.h
 * Author: Tobias Winkler
 *
 * Created on 20. März 2015, 14:44
 */



#pragma once
#include "MultivariatePolynomial.h"
#include "../numbers/GFNumber.h"


namespace carl{
        
template<typename Integer>
class TaylorExpansion{
        
        typedef GFNumber<Integer> FiniteInt;
        typedef MultivariatePolynomial<FiniteInt> Polynomial;

        public:
        
        /* for a given polynomial p(x_1,...,x_n) calculate the coefficient of (x_v - a)^k in
         * the <x_v - a>-adic representation of p(x_1,...,x_n).
         * 
         * This is equivalent to calculating the "taylor series expansion"
         * of p about x_v = a, according to GCL92 (Chap. 6, Multivariate Taylor Series Representation)
         */
        static Polynomial ideal_adic_coeff(
                Polynomial& p,
                Variable::Arg x_v,
                FiniteInt a,
                std::size_t k) {
                
                assert(p.has(x_v));
                // the expansion finishes when the total degree of p in x_v is reached
                if(k > p.degree(x_v)) {
                        return Polynomial(0);
                }
                Polynomial value = Polynomial(a);
                Polynomial ideal = Polynomial({(Term<FiniteInt>)x_v, Term<FiniteInt>(-a)});
                // error term and it's initialization
                Polynomial e;
                e = p - substitute(p,x_v, value);
                // curr coeff being calculated
                // when the calculation terminates this will contatin the result
                Polynomial u;
                u = Polynomial(0);
                // now we iterate: u_k = phi_I[(e_(k-1) - u_(k-1)*(x_v - a)^(k-1)) / (x_v - a)^k],
                // where phi_I[...] denotes the substitution of x_v by a.
                Polynomial temp;
                for(unsigned i = 1; i <= k; i++) {
                        e = e - u * ideal.pow(i-1);
                        temp = e / ideal.pow(i);
                        u = substitute(temp,x_v, value);
                }
                return u;
        }
};

} // namespace carl