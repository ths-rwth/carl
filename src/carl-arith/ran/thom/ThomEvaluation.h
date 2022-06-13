/* 
 * File:   ThomEvaluation.h
 * Author: tobias
 *
 * Created on 29. August 2016, 18:40
 */

#pragma once

#include "ran_thom.h"

namespace carl {

/*
 * returns the sign (NOT the actual evaluation) of p on the point defined by m as an RAN
 */
template<typename Number>
RealAlgebraicNumber<Number> evaluateTE(const MultivariatePolynomial<Number>& p, std::map<Variable, RealAlgebraicNumber<Number>>& m) {
        //using Polynomial = MultivariatePolynomial<Number>;
        
        CARL_LOG_INFO("carl.thom.evaluation",
                "\n****************************\n"
                << "Thom evaluate\n"
                << "****************************\n"
                << "p = " << p << "\n"
                << "m = " << m << "\n"
                << "****************************\n");
        for(const auto& entry : m) { 
                assert(entry.second.isThom());
                assert(entry.first == entry.second.getThomEncoding().main_var());
        }
        assert(m.size() > 0);
        
        std::map<Variable, RealAlgebraicNumber<Number>>& m_prime(m);
        auto it = m_prime.begin();
        while(it != m_prime.end()) {
                if(!p.has(it->first)) {
                        CARL_LOG_TRACE("carl.thom.evaluation", "removing " << it->first);
                        it = m_prime.erase(it);
                        
                }
                else {
                        it++;
                }
        }
        
        std::map<Variable, ThomEncoding<Number>> mTE;
        for(const auto& entry : m_prime) {
                mTE.insert(std::make_pair(entry.first, entry.second.getThomEncoding()));
        }
        
        CARL_LOG_ASSERT("carl.thom.evaluation", variables(p).size() == mTE.size(), "p = " << p << ", mTE = " << mTE);
        
        if(mTE.size() == 1) {
                int sgn = int(mTE.begin()->second.signOnPolynomial(p));
                CARL_LOG_TRACE("carl.thom.evaluation", "sign of evaluated polynomial is " << sgn);
                return RealAlgebraicNumber<Number>(Number(sgn), false);
        }
        
        CARL_LOG_TRACE("carl.thom.evaluation", "mTE = " << mTE);
        
        ThomEncoding<Number> point = ThomEncoding<Number>::analyzeTEMap(mTE);
        int sgn = int(point.signOnPolynomial(p));
        CARL_LOG_TRACE("carl.thom.", "sign of evaluated polynomial is " << sgn);
        return RealAlgebraicNumber<Number>(Number(sgn), false);
        
//        std::list<Polynomial> polynomials;
//        std::list<Polynomial> derivatives;
//        SignCondition sc;
//        for(const auto& entry : mTE) {
//                polynomials.push_front(entry.second.polynomial());
//                std::list<Polynomial> der = entry.second.relevantDerivatives();
//                derivatives.insert(derivatives.begin(), der.begin(), der.end());
//                SignCondition relSign = entry.second.relevantSignCondition();
//                sc.insert(sc.begin(), relSign.begin(), relSign.end());
//        }
//        
//        CARL_LOG_TRACE("carl.thom.evaluation", "polynomials = " << polynomials);
//        CARL_LOG_TRACE("carl.thom.evaluation", "derivatives = " << derivatives);
//        CARL_LOG_TRACE("carl.thom.evaluation", "sc = " << sc);
//                
//        SignDetermination<Number> sd(polynomials.begin(), polynomials.end());
//        sd.getSignsAndAddAll(derivatives.rbegin(), derivatives.rend());
//        std::list<SignCondition> signConditions = sd.getSigns(p);
//        CARL_LOG_TRACE("carl.thom.evaluation", "signConditions =  " << signConditions);
//        for(const auto sigma : signConditions) {
//                if(sc.isSuffixOf(sigma)) {
//                        int sgn = (int)sigma.front();
//                        CARL_LOG_TRACE("carl.thom.", "sign of evaluated polynomial is " << sgn);
//                        return RealAlgebraicNumber<Number>((Number)sgn, false);
//                }
//        }
//        
//        CARL_LOG_ASSERT("carl.thom.evaluation", false, "we should never get here");
}

        /*
        ThomEncoding<Number> point = std::max_element(m_prime.begin(), m_prime.end(),
                        [](const std::pair<Variable, RealAlgebraicNumber<Number>>& lhs, const std::pair<Variable, RealAlgebraicNumber<Number>>& rhs) {
                                return lhs.second.getThomEncoding().dimension() < rhs.second.getThomEncoding().dimension();
                        }
        )->second.getThomEncoding();
        
        CARL_LOG_TRACE("carl.thom.evaluation", "point with max dimension: " << point);

        if(m.size() == 1) {
                RealAlgebraicNumber<Number> ran = m.begin()->second;
                assert(ran.isThom());
                int sgn = (int)ran.getThomEncoding().signOnPolynomial(p);
                CARL_LOG_TRACE("carl.thom.evaluation", "sign of evaluated polynomial is " << sgn);
                return RealAlgebraicNumber<Number>((Number)sgn, false);
                
        }
        else if(m.size() == 2) {
                auto it = m.begin();
                RealAlgebraicNumber<Number> ran1 = it->second;
                it++;
                RealAlgebraicNumber<Number> ran2 = it->second;
                assert(ran1.getThomEncoding().point() == ran2.getThomEncoding());
                int sgn = (int)ran1.getThomEncoding().signOnPolynomial(p);
                CARL_LOG_TRACE("carl.thom.", "sign of evaluated polynomial is " << sgn);
                return RealAlgebraicNumber<Number>((Number)sgn, false);
        }
        else {
                assert(false);
                return RealAlgebraicNumber<Number>();
        }
}       */

} // namespace carl;
