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
        
        /* for a given polynomial p(x_1,...,x_n) calculate the coefficient of (x_v - a)^k in
         * the <x_v - a>-adic representation of p(x_1,...,x_n).
         * 
         * This is equivalent to calculating the "taylor series expansion"
         * of p about x_v = a, according to GCL92.
         */
        template<typename Integer>
        MultivariatePolynomial<GFNumber<Integer>> ideal_adic_coeff(
                MultivariatePolynomial<GFNumber<Integer>>& p,
                Variable::Arg x_v,
                GFNumber<Integer> a,
                std::size_t k) {
                
                assert(p.has(x_v));
                // the expansion finishes when the total degree of p in x_v is reached
                // (hence the coefficients will become 0)
                if(k > p.degree(x_v)) {
                        return MultivariatePolynomial<GFNumber<Integer>>(0);
                }
                MultivariatePolynomial<GFNumber<Integer>> value = MultivariatePolynomial<GFNumber<Integer>>(a);
                MultivariatePolynomial<GFNumber<Integer>> ideal = x_v - value;
                // error term and it's initialization
                MultivariatePolynomial<GFNumber<Integer>> e;
                e = p - p.substitute(x_v, value);
                // curr coeff being calculated
                // when the calculation terminates this will contatin the result
                MultivariatePolynomial<GFNumber<Integer>> u;
                u = MultivariatePolynomial<GFNumber<Integer>>(0);
                // now we iterate: u_k = phi_I[(e_(k-1) - u_(k-1)*(x_v - a)^(k-1)) / (x_v - a)^k],
                // where phi_I[...] denotes the substitution of x_v by a.
                MultivariatePolynomial<GFNumber<Integer>> temp;
                for(unsigned i = 1; i <= k; i++) {
                        e = e - u * ideal.pow(i-1);
                        temp = e / ideal.pow(i);
                        u = temp.substitute(x_v, value);
                }
                return u;
        }
}