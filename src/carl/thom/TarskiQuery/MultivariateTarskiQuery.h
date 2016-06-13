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
        BaseRepr<Coeff> q = normalForm(Q, gb);
        std::vector<_Monomial<Coeff>> base = table.getBase();
        // compute the traces...
        Matrix<Coeff> m(base.size(), base.size());
        for(int i = 0; i < base.size(); i++) {
                for(int j = 0; j < base.size(); j++) {
                        MultivariatePolynomial<Coeff> Qc = Q * base[i] * base[j];
                        BaseRepr<Coeff> nf_Qc = normalForm(Qc, gb);
                        Coeff t = trace(nf_Qc, table);
                        std::cout << t << "\t";
                        m(i, j) = t;
                }
                std::cout << std::endl;
        }
        std::vector<Coeff> cp = charPol(m);
        std::cout << "char pol: " << cp << std::endl;
        int v1 = (int)signVariations(cp.begin(), cp.end(), sgn<Coeff>);
        std::cout << v1 << std::endl;
        for(int i = 1; i < cp.size(); i += 2) {
                cp[i] *= -Coeff(1);
        }
        std::cout << "char pol': " << cp << std::endl;
        int v2 = (int)signVariations(cp.begin(), cp.end(), sgn<Coeff>);
        std::cout << v2 << std::endl;
        
        return v1 - v2;
}

} // namespace carl
