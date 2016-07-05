/* 
 * File:   ThomEncoding.tpp
 * Author: tobias
 *
 * Created on 2. Mai 2016, 22:06
 */

#pragma once

#include <Eigen/Dense>
#include <cmath>
#include <algorithm>

#include "SignDetermination/SignDetermination.h"
#include "ThomUtil.h"

using namespace Eigen;
using namespace carl;


/*
 * Implementation of class ThomEncoding
 * - constructors
 * - general member functions
 * - comparison operators for both one - and multidimensional case
 * - intermediate points and points smaller/greater than the repr. RAN
 * - ...
 */


//////////////////
// Constructors //
//////////////////

template<typename Coeff>
ThomEncoding<Coeff>::ThomEncoding(
                std::shared_ptr<UnivariatePolynomial<Coeff>> ptr,
                const SignCondition& s
) {
        CARL_LOG_FUNC("carl.thom", "");
        CARL_LOG_ASSERT("carl.thom", ptr->isUnivariate(), "");
        MultivariatePolynomial<Coeff> mp(*ptr);
        p = std::make_shared<MultivariatePolynomial<Coeff>>(mp);
        signs = s;
        mainVar = ptr->mainVar();
        CARL_LOG_ASSERT("catl.thom", isConsistent(), "");
}

template<typename Coeff>
ThomEncoding<Coeff>::ThomEncoding(
                std::shared_ptr<MultivariatePolynomial<Coeff>> ptr, 
                const SignCondition& s
) {
        CARL_LOG_FUNC("carl.thom", "");
        std::set<Variable> vars = ptr->gatherVariables();
        CARL_LOG_ASSERT("carl.thom", vars.size() == 1, "");
        p = ptr;
        signs = s;
        mainVar = *(vars.begin());
        CARL_LOG_ASSERT("carl.thom", isConsistent(), "");
}

template<typename Coeff>
ThomEncoding<Coeff>::ThomEncoding(
                std::shared_ptr<MultivariatePolynomial<Coeff>> ptr,
                Variable::Arg var,
                const SignCondition& s,
                std::shared_ptr<ThomEncoding<Coeff>> prev
) {
        CARL_LOG_FUNC("carl.thom", "ptr = " << ptr << ", var = " << var << ", s = " << s << ", prev = " << prev);
        p = ptr;
        mainVar = var;
        signs = s;
        point = prev;
        CARL_LOG_ASSERT("carl.thom", isConsistent(), "");
}

template<typename Coeff>
ThomEncoding<Coeff>::ThomEncoding(const Coeff& rational, Variable::Arg var) {
        CARL_LOG_FUNC("carl.thom", "rational = " << rational << ", var = " << var);
        MultivariatePolynomial<Coeff> polynomial = MultivariatePolynomial<Coeff>(var) - rational;
        std::shared_ptr<MultivariatePolynomial<Coeff>> ptr = std::make_shared<MultivariatePolynomial<Coeff>>(polynomial);
        SignCondition s = {};
        p = ptr;
        signs = s;
        mainVar = var;
        CARL_LOG_ASSERT("carl.thom", isConsistent(), "");
}


/////////////////////////////////////
// general public member functions //
/////////////////////////////////////

template<typename Coeff>
bool ThomEncoding<Coeff>::isConsistent() const {
        if(!p) {
                std::cout << "p is not initialized" << std::endl;
                return false;               
        }
        if(signs.size() != p->degree(mainVar) - 1) { 
                std::cout << "SignCondition has incorrect number of elements" << std::endl;
                return false;             
        }
        if(isOneDimensional()) {
                if(!p->isUnivariate()) {   
                        std::cout << "in the 1-dim case p must be univariate" << std::endl;
                        return false;                       
                }
                if(*(p->gatherVariables().begin()) != mainVar) {
                        std::cout << "wrong main variable" << std::endl;
                        return false;
                }
                if(tarskiQuery(UnivariatePolynomial<Coeff>(mainVar, {Coeff(1)}), polynomial().toUnivariatePolynomial()) == 0) { 
                        std::cout << "p has no roots - this is not ok" << std::endl;
                        return false;                 
                }             
                return true;
        }
        else {                                          // p is multidimensional
                assert(point);
                if(p->gatherVariables().find(mainVar) == p->gatherVariables().end()) {
                        std::cout << "main variable is not contained in p" << std::endl;
                        return false; 
                }
                if(this->mainVar == point->mainVar) {
                        std::cout << "the main variables of the encodings in a hierarchy must be different" << std::endl;
                        return false;
                }             
                return point->isConsistent();           // recursively check consistency of the levels below
        }
}

template<typename Coeff>
bool ThomEncoding<Coeff>::represents(const Coeff& rational) const {
        std::map<Variable, Coeff> evalMap = {std::make_pair(mainVar, rational)};
        // a necessary conditions is that the given number must be a root of the polynomial
        if(!isZero(p->evaluate(evalMap))) {
                return false;
        }
        // otherwise we check if the sign conditions all hold
        std::vector<MultivariatePolynomial<Coeff>> derivatives = der(p->derivative(mainVar), mainVar);
        assert(derivatives.size() == p->degree(mainVar));
        assert(derivatives.size() == signs.size() + 1);
        for(uint n = 0; n < derivatives.size(); n++) {
                if(Sign(sgn(derivatives[n].evaluate(evalMap))) != (*this)[n+1]) {
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
        res.push_back((*this)[p->degree(mainVar)]);
        return res;
}

template<typename Coeff>
SignCondition ThomEncoding<Coeff>::accumulateSigns() const {
        if(isOneDimensional()) {
                return signs;
        }
        else {
                SignCondition res = point->accumulateSigns();
                res.insert(res.end(), signs.begin(), signs.end());
                return res;
        }
}

template<typename Coeff>
uint ThomEncoding<Coeff>::dimension() const {
        if(isOneDimensional()) {
                return 1;
        }
        else {
                return point->dimension() + 1;
        }
}

template<typename Coeff>
std::vector<MultivariatePolynomial<Coeff>> ThomEncoding<Coeff>::accumulatePolynomials() const {
        if(isOneDimensional()) {
                return {polynomial()};
        }
        else {
                std::vector<MultivariatePolynomial<Coeff>> res = point->accumulatePolynomials();
                res.push_back(polynomial());
                assert(dimension() == res.size());
                return res;
        }       
}

template<typename Coeff>
std::vector<MultivariatePolynomial<Coeff>> ThomEncoding<Coeff>::reducedDer() const {
        std::vector<MultivariatePolynomial<Coeff>> res = der(polynomial(), mainVar, polynomial().degree(mainVar) - 1);
        res.erase(res.begin());
        return res;
}

template<typename Coeff>
std::vector<MultivariatePolynomial<Coeff>> ThomEncoding<Coeff>::accumulateDer() const {
        if(isOneDimensional()) {
                return reducedDer();
        }
        else {
                std::vector<MultivariatePolynomial<Coeff>> res = point->accumulateDer();
                std::vector<MultivariatePolynomial<Coeff>> der = reducedDer();
                res.insert(res.end(), der.begin(), der.end());
                return res;
        }
}


////////////////
// Comparison //
////////////////

// compares one dimensional thom encodings
template<typename Coeff>
ComparisonResult compareUnivariate(const ThomEncoding<Coeff>& lhs, const ThomEncoding<Coeff>& rhs) {
        assert(lhs.isOneDimensional() && rhs.isOneDimensional());
        // assert that the main variables are the same ???
        if(lhs.polynomial() == rhs.polynomial()) {
                
                // --- same underlying polynomials ---
                
                // trivial case...
                if(lhs.getSigns() == rhs.getSigns()) {
                        return EQUAL;
                }
                
                // Algorithms for Real Algebraic Geometry, Proposition 2.28, p. 42
                if(lhs.fullSignCondition() < rhs.fullSignCondition()) {
                        return LESS;
                }
                else if(lhs.fullSignCondition() > rhs.fullSignCondition()) {
                        return GREATER;
                }
                else {
                        return EQUAL;
                }
        }
        else {
                
                // --- different underlying polynomials ---
                
                std::vector<UnivariatePolynomial<Coeff>> der1 = der(lhs.polynomial().toUnivariatePolynomial());
                std::vector<UnivariatePolynomial<Coeff>> der2 = der(rhs.polynomial().toUnivariatePolynomial().derivative());
                der2.pop_back(); // do this nicer
                der1.insert(der1.end(), der2.begin(), der2.end());
                PRINTV(der1);
                std::vector<SignCondition> signConds = signDetermination(der1, rhs.polynomial().toUnivariatePolynomial()); 
                SignCondition wanted; // linear search here...
                bool succes = false;
                for(const SignCondition& c : signConds) {
                        if(extends(c, rhs.getSigns())) {
                                wanted = c;
                                succes = true;
                                break;
                        }
                }
                assert(succes);
                wanted.erase(wanted.begin() + lhs.polynomial().degree(lhs.getMainVar()) + 1, wanted.end());
                PRINTV(wanted);
                PRINTV(lhs.getSigns());
                assert(wanted.size() == lhs.getSigns().size() + 2);
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
ComparisonResult compareMultivariate(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        assert(lhs.isMultiDimensional() && rhs.isMultiDimensional());
        assert(lhs.dimension() == rhs.dimension());
        assert(lhs.point->accumulatePolynomials() == rhs.point->accumulatePolynomials());
        assert(lhs.point->accumulateSigns() == rhs.point->accumulateSigns());
        if(lhs.p == rhs.p) {
                            
                if(lhs.fullSignCondition() < rhs.fullSignCondition()) {
                        return LESS;
                }
                else if(lhs.fullSignCondition() > rhs.fullSignCondition()) {
                        return GREATER;
                }
                else {
                        return EQUAL;
                }
        }
        else {
                // --- different underlying polynomials ---
                
                std::vector<MultivariatePolynomial<C>> deriv = lhs.accumulateDer();
                std::vector<MultivariatePolynomial<C>> der_rhs = der(rhs.polynomial(), rhs.getMainVar());
                deriv.insert(deriv.end(), der_rhs.begin(), der_rhs.end());
                PRINTV(deriv);
                
                std::vector<MultivariatePolynomial<C>> zeroSet = lhs.accumulatePolynomials();
                std::vector<SignCondition> signConds = signDetermination(deriv, zeroSet); 
                SignCondition lhs_signs = lhs.accumulateSigns();
                PRINTV(signConds);
                PRINTV(lhs_signs);
                
                SignCondition wanted; // linear search here...
                bool succes = false;
                for(const SignCondition& s : signConds) {
                        if(isPrefix(lhs_signs, s)) {
                                wanted = s;
                                succes = true;
                                break;
                        }
                }
                assert(succes);
                PRINTV(wanted);
                wanted.erase(wanted.begin(), wanted.begin() + lhs_signs.size());
                PRINTV(wanted);

                PRINTV(rhs.getSigns());
                PRINTV(rhs.fullSignCondition());

                
                if(wanted < rhs.fullSignCondition()) {
                        return LESS;
                }
                else if(wanted == rhs.fullSignCondition()) {
                        return EQUAL;
                }
                else {
                        return GREATER;
                }                  
        }
        return EQUAL;
}

// compares two RANs represented by thom encodings
template<typename C>
ComparisonResult compareThom(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        if(lhs.isOneDimensional() && rhs.isOneDimensional()) {
                return compareUnivariate(lhs, rhs);
        }
        else if(lhs.isMultiDimensional() && rhs.isMultiDimensional()) {
                return compareMultivariate(lhs, rhs);
        }
        // we can NOT compare encodings on different levels
        else {
                CARL_LOG_FATAL("carl.thom", "tried to compare encodings on different levels");
                assert(false);
                // this is just to surpress a warning
                return EQUAL;
        }              
}


/////////////////////////
// Intermediate points //
/////////////////////////

template<typename C>
ThomEncoding<C> intermediatePoint(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        CARL_LOG_ASSERT("carl.thom", lhs < rhs, "");
        C epsilon = C(1);
        ThomEncoding<C> res = lhs + epsilon;
        while(res >= rhs) {
                epsilon /= 2;
                res = lhs + epsilon;
        }
        return res;
}

template<typename C>
ThomEncoding<C> operator+(const ThomEncoding<C>& rhs, const C& lhs) {
        MultivariatePolynomial<C> subs(rhs.mainVar);
        subs -= lhs;
        MultivariatePolynomial<C> newPoly = rhs.p->substitute(rhs.mainVar, subs);
        ThomEncoding<C> result = rhs;
        result.p = std::make_shared<MultivariatePolynomial<C>>(newPoly);
        return result;
}

