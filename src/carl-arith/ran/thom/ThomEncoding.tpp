/* 
 * File:   ThomEncoding.tpp
 * Author: tobias
 *
 * Created on 20. August 2016, 11:37
 */

#include "ThomEncoding.h"


#pragma once

namespace carl {
        
//*******************//
//    COMPARISON     //
//*******************//
       
/*
 * called by ThomEncoding::compare
 * don't call directly
 */
template<typename Number>
ThomComparisonResult ThomEncoding<Number>::compareDifferentPoly(const ThomEncoding<Number>& lhs, const ThomEncoding<Number>& rhs) {
        using Polynomial = MultivariatePolynomial<Number>;

        std::list<Polynomial> der_rhs = der(rhs.polynomial(), rhs.main_var(), 0, rhs.polynomial().degree(rhs.main_var()));
        SignCondition sc_lhs_on_der_rhs;
        auto it_der_rhs = der_rhs.rbegin();
        uint currLength = 1;
        for( ; it_der_rhs != der_rhs.rend(); it_der_rhs++) {
                sc_lhs_on_der_rhs.push_front(lhs.signOnPolynomial(*it_der_rhs));
                if(rhs.mSc.size() < currLength) rhs.extendSignCondition();
                SignCondition sc_rhs_curr = rhs.mSc.trailingPart(currLength);
                if(sc_rhs_curr != sc_lhs_on_der_rhs) {
                        CARL_LOG_TRACE("carl.thom.compare", "comparing " << sc_lhs_on_der_rhs << " with " << sc_rhs_curr);
                        return SignCondition::compare(sc_lhs_on_der_rhs, sc_rhs_curr);
                }

                currLength++;
        }
        // that means that lhs realized precisely the same signs on der_rhs as rhs itself -> then the encoded numbers must be equal
        return EQUAL;              
}
                
} // namespace carl
