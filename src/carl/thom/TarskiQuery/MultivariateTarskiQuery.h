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
        if(Q.isUnivariate()) {
                CARL_LOG_WARN("carl.thom.tarski", "input to multivariateTarskiQuery is a univariate polynomial!!");
        }
        CARL_LOG_INEFFICIENT();
        BaseRepr<Coeff> q = normalForm(Q, gb);
        std::vector<_Monomial<Coeff>> base = table.getBase();
        // compute the traces...
        CoeffMatrix<Coeff> m(base.size(), base.size());
        for(int i = 0; i < base.size(); i++) {
                for(int j = 0; j < base.size(); j++) {
                        MultivariatePolynomial<Coeff> Qc = Q * base[i] * base[j];
                        BaseRepr<Coeff> nf_Qc = normalForm(Qc, gb);
                        Coeff t = trace(nf_Qc, table);
                        m(i, j) = t;
                }
        }
        std::vector<Coeff> cp = charPol(m);
        CARL_LOG_TRACE("carl.thom.tarski", "char pol: " << cp);
        int v1 = (int)signVariations(cp.begin(), cp.end(), sgn<Coeff>);
        for(int i = 1; i < cp.size(); i += 2) {
                cp[i] *= -Coeff(1);
        }
        CARL_LOG_TRACE("carl.thom.tarski", "char pol': " << cp);
        int v2 = (int)signVariations(cp.begin(), cp.end(), sgn<Coeff>);
        
        CARL_LOG_TRACE("carl.thom.tarski", "result = " << v1 - v2);
        return v1 - v2;
}

} // namespace carl
