/* 
 * File:   ThomEncoding.tpp
 * Author: tobias
 *
 * Created on 2. Mai 2016, 22:06
 */


#pragma once

#include <Eigen/Dense>
#include <cmath>

#include "SignDetermination.h"
#include "ThomUtil.h"




using namespace Eigen;
using namespace carl;


template<typename Coeff>
ThomEncoding<Coeff>::ThomEncoding(const std::shared_ptr<UnivariatePolynomial<Coeff>>& ptr, const SignCondition& s) : 
        p(ptr), signs(s)
{
        assert(isConsistent());
}

template<typename Coeff>
bool ThomEncoding<Coeff>::represents(Coeff rational) const {
        if(!isZero(p->evaluate(rational))) {
                return false;
        }
        std::vector<UnivariatePolynomial<Coeff>> derivatives = der(p->derivative());
        assert(derivatives.size() == p->degree());
        assert(derivatives.size() == signs.size() + 1);
        for(uint n = 0; n < derivatives.size(); n++) {
                if(Sign(sgn(derivatives[n].evaluate(rational))) != (*this)[n+1]) {
                        return false;
                }
        }
        return true;
}

template<typename Coeff>
Sign ThomEncoding<Coeff>::operator[](const uint n) const {
        if(n == 0) {
                return Sign::ZERO;
        }
        else if(n <= (unsigned)signs.size()) {
                return signs[n-1];
        }
        else if(n == (unsigned)signs.size() + 1) {
                return Sign(sgn(p->lcoeff()));
        }
        else {
                return Sign::ZERO;
        }
}

template<typename Coeff>
SignCondition ThomEncoding<Coeff>::fullSignCondition() const {
        SignCondition res = signs;
        res.insert(res.begin(), (*this)[0]);
        assert(res[0] == Sign::ZERO);
        res.push_back((*this)[p->degree()]);
        return res;
}


// compares two RANs represented by thom encodings
template<typename C>
ComparisonResult compare(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        assert(lhs.p != nullptr && rhs.p != nullptr);
        // --- same underlying polynomials ---
        // see Algorithms for Real Algebraic Geometry, Proposition 2.28, p. 42
        if(*(lhs.p) == *(rhs.p)) {                              
                assert(lhs.signs.size() == rhs.signs.size());
                for(uint k = lhs.p->degree(); k > 0; k--) {
                        if(lhs[k] != rhs[k]) {
                                Sign equalSign = lhs[k+1];
                                assert(equalSign == rhs[k+1]);
                                assert(equalSign != Sign::ZERO);
                                if(equalSign == Sign::POSITIVE) {
                                        return lhs[k] < rhs[k] ? LESS : GREATER;
                                }
                                else { // equalSign == Sign::NEGATIVE
                                        return lhs[k] > rhs[k] ? LESS : GREATER;
                                }
                        }
                }
                // in this case they are actually equal
                return EQUAL;
        }
        // --- different underlying polynomials ---
        else {                                                  
                std::vector<UnivariatePolynomial<C>> der1 = der(*(lhs.p));
                std::vector<UnivariatePolynomial<C>> der2 = der(rhs.p->derivative());
                der2.pop_back(); // do this nicer
                der1.insert(der1.end(), der2.begin(), der2.end());
                PRINTV(der1);
                std::vector<SignCondition> signConds = signDetermination(der1, *(rhs.p));
                SignCondition wanted; // linear search here...
                bool succes = false;
                for(const SignCondition& c : signConds) {
                        if(extends(c, rhs.signs)) {
                                wanted = c;
                                succes = true;
                                break;
                        }
                }
                assert(succes);
                wanted.erase(wanted.begin() + lhs.p->degree() + 1, wanted.end());
                PRINTV(wanted);
                PRINTV(lhs.signs);
                assert(wanted.size() == lhs.signs.size() + 2);
                PRINTV(wanted);
                PRINTV(lhs.fullSignCondition());

                if(lhs.fullSignCondition() < wanted) {
                        return LESS;
                }
                else if(lhs.fullSignCondition() == wanted) {
                        return EQUAL;
                }
                else {
                        return GREATER;
                }
        }
}


template<typename C>
bool operator<(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) { 
        return compare(lhs, rhs) == LESS;
}

template<typename C>
bool operator<=(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        return compare(lhs, rhs) != GREATER;
}

template<typename C>
bool operator>(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        return compare(lhs, rhs) == GREATER;
}

template<typename C>
bool operator>=(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        return compare(lhs, rhs) != LESS;
}

template<typename C>
bool operator==(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        return compare(lhs, rhs) == EQUAL;
}

template<typename C>
bool operator!=(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        return !(lhs == rhs);
}