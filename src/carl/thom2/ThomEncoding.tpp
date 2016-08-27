/* 
 * File:   ThomEncoding.tpp
 * Author: tobias
 *
 * Created on 20. August 2016, 11:37
 */

#include "ThomEncoding.h"


#pragma once

namespace carl {
        
template<typename N>
ThomComparisonResult compare(const ThomEncoding<N>& lhs, const ThomEncoding<N>& rhs) {
        CARL_LOG_ASSERT("carl.thom.compare", lhs.mPoint == rhs.mPoint, "compare on non-comparable thom encodings called");
        if(lhs.polynomial() == rhs.polynomial()) return compare(lhs.signCondition(), rhs.signCondition());
        using Polynomial = MultivariatePolynomial<N>;
        
        SignCondition sc_lhs = lhs.signCondition();
        SignCondition allSigns_lhs = lhs.accumulateSigns();
        std::list<Polynomial> der_rhs = der(rhs.polynomial(), rhs.mainVar(), 1, rhs.polynomial().degree(rhs.mainVar()));
        
        SignDetermination<N> tmp(lhs.sd());
        auto it_der_rhs = der_rhs.rbegin();
        tmp.getSignsAndAdd(*it_der_rhs);
        it_der_rhs++;
        std::list<SignCondition> signs = tmp.getSignsAndAdd(*it_der_rhs);
        
        SignCondition sc_rhs;
        for(const auto& s : signs) {
                if(allSigns_lhs.isSuffixOf(s)) {
                        sc_rhs = s;
                        break;
                }
        }
        sc_rhs.resize(2);
        
        std::cout << "rhs.mSc = " << rhs.mSc << std::endl;
        std::cout << "sc_rhs = " << sc_rhs << std::endl;
        
        
        
        
        
        
        return EQUAL;
                
}
        
        
        
} // namespace carl
