/* 
 * File:   TarskiQuery.h
 * Author: tobias
 *
 * Created on 24. April 2016, 20:16
 */

#pragma once

#include "../../core/UnivariatePolynomial.h"
#include "../../core/Sign.h"

namespace carl {

template<typename Coeff>
Sign signAtMinusInf(const UnivariatePolynomial<Coeff>& p) {
        if(p.isZero()) return Sign::ZERO; // check this special case because otherwise lcoeff fires assertion
        if(p.degree() % 2 == 0) {               // even degree
                return carl::sgn(p.lcoeff());
        }
        else {                                  // odd degree
                return carl::sgn(-p.lcoeff());
        }
}

template<typename Coeff>
Sign signAtPlusInf(const UnivariatePolynomial<Coeff>& p) {
        if(p.isZero()) return Sign::ZERO;
        return carl::sgn(p.lcoeff());
}


/*
 * implements the signed remainder sequence tarski query as described in algorithms for real algebraic geometry
 * TODO: also implement the subresult tarski query
 */
template<typename Coeff>
int univariateTarskiQuery(const UnivariatePolynomial<Coeff>& p, const UnivariatePolynomial<Coeff>& q, const UnivariatePolynomial<Coeff>& der_q) {
        assert(!q.isZero());
        std::list<UnivariatePolynomial<Coeff>> srs = q.standardSturmSequence(der_q * p); // standardSturmSequence is the signed remainder sequence
        int atMinusInf = (int)signVariations(srs.begin(), srs.end(), [](const UnivariatePolynomial<Coeff>& p){ return signAtMinusInf(p); }); 
        int atPlusInf = (int)signVariations(srs.begin(), srs.end(), [](const UnivariatePolynomial<Coeff>& p){ return signAtPlusInf(p); });
        return atMinusInf - atPlusInf;
}

template<typename Coeff>
int univariateTarskiQuery(const UnivariatePolynomial<Coeff>& p, const UnivariatePolynomial<Coeff>& q) {
        return univariateTarskiQuery(p, q, q.derivative());
}

} // namespace carl




