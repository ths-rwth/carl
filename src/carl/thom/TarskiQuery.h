/* 
 * File:   TarskiQuery.h
 * Author: tobias
 *
 * Created on 24. April 2016, 20:16
 */

#pragma once

#include "../core/UnivariatePolynomial.h"
#include "../core/Sign.h"

template<typename Coeff>
carl::Sign signAtMinusInf(const carl::UnivariatePolynomial<Coeff>& p) {
        if(p.isConstant()) return carl::sgn(p.constantPart());
        if(p.degree() % 2 == 0) {               // even degree
                return carl::sgn(p.lcoeff());
        }
        else {                                  // odd degree
                return carl::sgn(-p.lcoeff());
        }
}

template<typename Coeff>
carl::Sign signAtPlusInf(const carl::UnivariatePolynomial<Coeff>& p) {
        if(p.isConstant()) return carl::sgn(p.constantPart());
        if(p.degree() % 2 == 0) {               // even degree
                return carl::sgn(p.lcoeff());
        }
        else {                                  // odd degree
                return carl::sgn(p.lcoeff());
        }
}



template<typename Coeff>
int tarskiQuery(const carl::UnivariatePolynomial<Coeff>& p, const carl::UnivariatePolynomial<Coeff>& q) {
        assert(!q.isZero());
        std::list<carl::UnivariatePolynomial<Coeff>> srs = q.standardSturmSequence(q.derivative() * p); // standardSturmSequence is the signed remainder sequence
        int atMinusInf = (int)carl::signVariations(srs.begin(), srs.end(), [](const carl::UnivariatePolynomial<Coeff>& p){ return signAtMinusInf(p); }); 
        int atPlusInf = (int)carl::signVariations(srs.begin(), srs.end(), [](const carl::UnivariatePolynomial<Coeff>& p){ return signAtPlusInf(p); });
        return atMinusInf - atPlusInf;
}


