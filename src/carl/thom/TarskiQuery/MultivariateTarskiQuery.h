/* 
 * File:   MultivariateTarskiQuery.h
 * Author: tobias
 *
 * Created on 11. Juni 2016, 11:45
 */

#pragma once

#include "../../core/Sign.h"
#include "CharPol.h"
#include "MultiplicationTable.h"

namespace carl {
        
        
template<typename Number>
int multivariateTarskiQuery(const MultivariatePolynomial<Number>& Q, const MultiplicationTable<Number>& table) {
        CARL_LOG_FUNC("carl.thom.tarski", "Q = " << Q);
        BaseRepresentation<Number> q = table.reduce(Q);
        std::vector<typename MultiplicationTable<Number>::Monomial> base = table.getBase();
        // compute the traces...
        CoeffMatrix<Number> m(base.size(), base.size());
        CARL_LOG_INFO("carl.thom.tarski", "base size is " << base.size());
        CARL_LOG_INFO("carl.thom.tarski", "setting up the matrix now ...");
        for(const auto& entry : table) {
                BaseRepresentation<Number> Qc = table.multiply(q, entry.second.br);
                Number t = table.trace(Qc);
                for (const auto& pair : entry.second.pairs) {
                        m(long(pair.first), long(pair.second)) = t;
                }
        }
        
        /*
        for(uint i = 0; i < base.size(); i++) {
                for(uint j = i; j < base.size(); j++) {    
                        BaseRepresentation<Number> prod = table.getEntry(base[i] * base[j]).br;
                        BaseRepresentation<Number> Qc = table.multiply(q, prod);
                        //BaseRepresentation<Number> Qc = table.reduce(base[i] * base[j] * Q);
                        Number t = table.trace(Qc);
                        m(i, j) = t;
                        m(j, i) = t;
                }
        }
        */
        /*
        for(uint i = 0; i < base.size(); i++) {
                for(uint j = i; j < base.size(); j++) {
                        std::cout << m(i, j) << "\t\t";
                }
                std::cout << std::endl;
        }
        std::cout << std::endl;
        */
        
        CARL_LOG_INFO("carl.thom.tarski", "... done setting up matrix.");
        std::vector<Number> cp = charPol(m);
        CARL_LOG_TRACE("carl.thom.tarski", "char pol: " << cp);
        int v1 = int(signVariations(cp.begin(), cp.end(), sgn<Number>));
        for(uint i = 1; i < cp.size(); i += 2) {
                cp[i] *= -Number(1);
        }
        CARL_LOG_TRACE("carl.thom.tarski", "char pol': " << cp);
        int v2 = int(signVariations(cp.begin(), cp.end(), sgn<Number>));
        
        CARL_LOG_TRACE("carl.thom.tarski", "result = " << v1 - v2);
        return v1 - v2;
}

} // namespace carl
