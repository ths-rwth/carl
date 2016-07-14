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
        CARL_LOG_ASSERT("carl.thom", isConsistent(), "");
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
ThomEncoding<Coeff>::ThomEncoding(
                const Coeff& rational,
                Variable::Arg var
) {
        CARL_LOG_FUNC("carl.thom", "rational = " << rational << ", var = " << var);
        MultivariatePolynomial<Coeff> polynomial = MultivariatePolynomial<Coeff>(var) - rational;
        std::shared_ptr<MultivariatePolynomial<Coeff>> ptr = std::make_shared<MultivariatePolynomial<Coeff>>(polynomial);
        SignCondition s = {};
        p = ptr;
        signs = s;
        mainVar = var;
        CARL_LOG_ASSERT("carl.thom", isConsistent(), "");
}

template<typename Coeff>
ThomEncoding<Coeff>::ThomEncoding(
                const Coeff& rational,
                Variable::Arg var,
                std::shared_ptr<ThomEncoding<Coeff>> point
) {
        MultivariatePolynomial<Coeff> polynomial = MultivariatePolynomial<Coeff>(var) - rational;
        std::shared_ptr<MultivariatePolynomial<Coeff>> ptr = std::make_shared<MultivariatePolynomial<Coeff>>(polynomial);
        SignCondition s = {};
        p = ptr;
        signs = s;
        mainVar = var;
        this->point = point;
        CARL_LOG_ASSERT("carl.thom", isConsistent(), "");
}


/////////////////////////////////////
// general public member functions //
/////////////////////////////////////

template<typename Coeff>
bool ThomEncoding<Coeff>::isConsistent() const {
        if(!p) {
                CARL_LOG_DEBUG("carl.thom", "p is not initialized");
                return false;               
        }
        if(signs.size() != p->degree(mainVar) - 1) { 
                CARL_LOG_DEBUG("carl.thom", "SignCondition has incorrect number of elements");
                return false;             
        }
        if(isOneDimensional()) {
                if(!p->isUnivariate()) {   
                        CARL_LOG_DEBUG("carl.thom", "in the 1-dim case p must be univariate");
                        return false;                       
                }
                if(*(p->gatherVariables().begin()) != mainVar) {
                        CARL_LOG_DEBUG("carl.thom", "wrong main variable");
                        return false;
                }
                if(tarskiQuery(UnivariatePolynomial<Coeff>(mainVar, {Coeff(1)}), polynomial().toUnivariatePolynomial()) == 0) { 
                        CARL_LOG_DEBUG("carl.thom", "p has no roots - this is not ok");
                        return false;                 
                }             
                return true;
        }
        else {                                          // p is multidimensional
                assert(point);
                if(p->gatherVariables().find(mainVar) == p->gatherVariables().end()) {
                        CARL_LOG_DEBUG("carl.thom", "main variable is not contained in p");
                        return false; 
                }
                if(this->mainVar == point->mainVar) {
                        CARL_LOG_DEBUG("carl.thom", "the main variables of the encodings in a hierarchy must be different");
                        return false;
                }             
                return point->isConsistent();           // recursively check consistency of the levels below
        }
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
                return Sign(sgn(p->derivative(mainVar).lcoeff()));
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
        std::vector<MultivariatePolynomial<Coeff>> res;
        if(polynomial().degree(mainVar) > 1) {
                res = der(polynomial(), mainVar, polynomial().degree(mainVar) - 1);
                res.erase(res.begin());
        }
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

template<typename Coeff>
Sign ThomEncoding<Coeff>::sgnReprNum() const {
        ThomComparisonResult compRes = compareRational(*this, constant_zero<Coeff>::get());
        if(compRes == LESS) return Sign::NEGATIVE;
        else if(compRes == GREATER) return Sign::POSITIVE;
        else return Sign::ZERO;
}

template<typename C>
bool areComparable(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        if(lhs.isOneDimensional() && rhs.isOneDimensional()) {
                return true;
        }
        else if(lhs.isMultiDimensional() && rhs.isMultiDimensional()) {
                if(lhs.point->accumulatePolynomials() != rhs.point->accumulatePolynomials()) return false;
                if(lhs.point->accumulateSigns() != rhs.point->accumulateSigns()) return false;
                return true;
        }
        return false;
}


////////////////
// Comparison //
////////////////

template<typename C>
ThomComparisonResult compareRational(const ThomEncoding<C>& t, const C& rat) {
        if(t.isOneDimensional()) {
                return compareThom(t, ThomEncoding<C>(rat, t.mainVar));
        }
        else {
                return compareThom(t, ThomEncoding<C>(rat, t.mainVar, t.point));
        }
}

// compares two RANs represented by thom encodings
template<typename C>
ThomComparisonResult compareThom(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        CARL_LOG_ASSERT("carl.thom", areComparable(lhs, rhs), "");
        if(lhs.isOneDimensional()) {
                return compareUnivariate(lhs, rhs);
        }
        else {
                return compareMultivariate(lhs, rhs);
        }             
}

// compares one dimensional thom encodings
template<typename C>
ThomComparisonResult compareUnivariate(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        // assert that the main variables are the same ???
        
        // --- same underlying polynomials ---    
        if(lhs.polynomial() == rhs.polynomial()) {         
                return lhs.fullSignCondition() < rhs.fullSignCondition();
        }
        
        // --- different underlying polynomials ---
        else {
                std::vector<UnivariatePolynomial<C>> der1 = der(lhs.polynomial().toUnivariatePolynomial());
                std::vector<UnivariatePolynomial<C>> der2 = der(rhs.polynomial().toUnivariatePolynomial().derivative());
                der2.pop_back(); // do this nicer
                der1.insert(der1.end(), der2.begin(), der2.end());
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
                assert(wanted.size() == lhs.getSigns().size() + 2);

                return lhs.fullSignCondition() < wanted;             
        }      
}

template<typename C>
ThomComparisonResult compareMultivariate(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        
        // --- same underlying polynomials --- 
        if(lhs.polynomial() == rhs.polynomial()) {
                return lhs.fullSignCondition() < rhs.fullSignCondition();
        }
        
        // --- different underlying polynomials ---
        else {
                std::vector<MultivariatePolynomial<C>> deriv = lhs.accumulateDer();
                std::vector<MultivariatePolynomial<C>> der_rhs = der(rhs.polynomial(), rhs.getMainVar());
                deriv.insert(deriv.end(), der_rhs.begin(), der_rhs.end());
                
                std::vector<MultivariatePolynomial<C>> zeroSet = lhs.accumulatePolynomials();
                std::vector<SignCondition> signConds = signDetermination(deriv, zeroSet); 
                SignCondition lhs_signs = lhs.accumulateSigns();
                
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
                wanted.erase(wanted.begin(), wanted.begin() + lhs_signs.size());
                
                return wanted < rhs.fullSignCondition();
        }
        return EQUAL;
}


/////////////////////////
// Intermediate points //
/////////////////////////

template<typename C, typename Settings = ThomDefaultSettings>
ThomEncoding<C> intermediatePoint(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        CARL_LOG_ASSERT("carl.thom", lhs < rhs, "please call intermediatePoint with the right order in the arguments");
        C epsilon = Rational(1);
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

