/* 
 * File:   MultivariateTarskiQuery.h
 * Author: tobias
 *
 * Created on 11. Juni 2016, 11:45
 */

#pragma once

#include "Trace.h"
#include "CharPol.h"
#include "../../core/Sign.h"

namespace carl {
        
// algorithm ???
template<typename Coeff>
int multivariateTarskiQuery(const MultivariatePolynomial<Coeff>& Q, const MultiplicationTable<Coeff>& table, const GB<Coeff>& gb) {
        CARL_LOG_FUNC("carl.thom.tarski", "Q = " << Q);
        BaseRepr<Coeff> q = normalForm(Q, gb);
        std::vector<_Monomial<Coeff>> base = table.getBase();
        // compute the traces...
        CoeffMatrix<Coeff> m(base.size(), base.size());
        CARL_LOG_INFO("carl.thom.tarski", "setting up the matrix now ...");
        CARL_LOG_WARN("carl.thom.tarski", "dont forget to improve 'multiply'");
        for(uint i = 0; i < base.size(); i++) {
                for(uint j = i; j < base.size(); j++) {
                        // SCHEINBAR IST DIESE METHODE IM MOMENT NOCH BESSER!!!
                        
                        //BaseRepr<Coeff> prod = table.get(base[i] * base[j]);
                        MultivariatePolynomial<Coeff> Qc = Q * base[i] * base[j];
                        BaseRepr<Coeff> nf_Qc = normalForm(Qc, gb);
                        Coeff t = trace(nf_Qc, table);
                        //BaseRepr<Coeff> Qc = multiply(q, prod, table);
                        //Coeff t = trace(Qc, table);
                        m(i, j) = t;
                        m(j, i) = t;
                }
        }
        CARL_LOG_INFO("carl.thom.tarski", "... done setting up matrix.");
        std::vector<Coeff> cp = charPol(m);
        CARL_LOG_TRACE("carl.thom.tarski", "char pol: " << cp);
        int v1 = (int)signVariations(cp.begin(), cp.end(), sgn<Coeff>);
        for(uint i = 1; i < cp.size(); i += 2) {
                cp[i] *= -Coeff(1);
        }
        CARL_LOG_TRACE("carl.thom.tarski", "char pol': " << cp);
        int v2 = (int)signVariations(cp.begin(), cp.end(), sgn<Coeff>);
        
        CARL_LOG_TRACE("carl.thom.tarski", "result = " << v1 - v2);
        return v1 - v2;
}

} // namespace carl
