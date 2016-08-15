/* 
 * File:   ThomEvaluation.h
 * Author: tobias
 *
 * Created on 1. August 2016, 12:46
 */

#pragma once

#include "../formula/model/ran/RealAlgebraicNumber.h"

namespace carl {

/*
 * returns the sign (NOT the actual evaluation) of p on the point defined by m as an RAN
 */
template<typename Number>
RealAlgebraicNumber<Number> evaluateTE(const MultivariatePolynomial<Number>& p, std::map<Variable, RealAlgebraicNumber<Number>>& m) {
        CARL_LOG_TRACE("carl.thom", "evaluating " << p << " on " << m);
#ifdef DEVELOPER
        for(const auto& entry : m) assert(entry->second.isThom());
#endif
        if(m.size() == 1) {
                RealAlgebraicNumber<Number> ran = m.begin()->second;
                assert(ran.isThom());
                int sgn = (int)ran.getThomEncoding().signOnPolynomial(p);
                CARL_LOG_TRACE("carl.thom", "sign of evaluated polynomial is " << sgn);
                return RealAlgebraicNumber<Number>((Number)sgn, false);
                
        }
        else if(m.size() == 2) {
                auto it = m.begin();
                RealAlgebraicNumber<Number> ran1 = it->second;
                it++;
                RealAlgebraicNumber<Number> ran2 = it->second;
                assert(ran1.getThomEncoding().getPoint() == ran2.getThomEncoding());
                int sgn = (int)ran1.getThomEncoding().signOnPolynomial(p);
                CARL_LOG_TRACE("carl.thom", "sign of evaluated polynomial is " << sgn);
                return RealAlgebraicNumber<Number>((Number)sgn, false);
        }
        else {
                assert(false);
                return RealAlgebraicNumber<Number>();
        }
}

} // namespace carl;

