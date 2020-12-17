/* 
 * File:   TarskiQuery.h
 * Author: tobias
 *
 * Created on 24. ApriU 2016, 20:16
 */

#pragma once

#include <carl/core/Sign.h>
#include <carl/core/UnivariatePolynomial.h>

namespace carl {

template<typename Number>
Sign signAtMinusInf(const UnivariatePolynomial<Number>& p) {
        if(carl::isZero(p)) return Sign::ZERO; // check this special case because otherwise lcoeff fires assertion
        if(p.degree() % 2 == 0) {               // even degree
                return carl::sgn(p.lcoeff());
        }
        else {                                  // odd degree
                return carl::sgn(Number(-p.lcoeff()));
        }
}

template<typename Number>
Sign signAtPlusInf(const UnivariatePolynomial<Number>& p) {
        if(carl::isZero(p)) return Sign::ZERO;
        return carl::sgn(p.lcoeff());
}


/*
 * implements the signed remainder sequence tarski query as described in algorithms for real algebraic geometry
 * TODO: offer version with bounds?
 */
template<typename Number>
int univariateTarskiQuery(const UnivariatePolynomial<Number>& p, const UnivariatePolynomial<Number>& q, const UnivariatePolynomial<Number>& der_q) {
        CARL_LOG_FUNC("carl.thom.tarski", "p = " << p << ", q = " << q << ", der_q = " << der_q);
        CARL_LOG_ASSERT("carl.thom.tarski", !carl::isZero(q), "cannot query on an infinite zero set!");
        auto srs = sturm_sequence(q, der_q * p); // standardSturmSequence is the signed remainder sequence
        int atMinusInf = int(sign_variations(srs.begin(), srs.end(), [](const UnivariatePolynomial<Number>& p){ return signAtMinusInf(p); })); 
        int atPlusInf = int(sign_variations(srs.begin(), srs.end(), [](const UnivariatePolynomial<Number>& p){ return signAtPlusInf(p); }));
        return atMinusInf - atPlusInf;
}

template<typename Number>
int univariateTarskiQuery(const UnivariatePolynomial<Number>& p, const UnivariatePolynomial<Number>& q) {
        return univariateTarskiQuery(p, q, q.derivative());
}

} // namespace carl
