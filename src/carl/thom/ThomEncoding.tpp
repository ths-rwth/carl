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
#include "ThomEncoding.h"

//using namespace Eigen;



/*
 * Implementation of class ThomEncoding
 * - constructors
 * - general member functions
 * - comparison operators for both one - and multidimensional case
 * - intermediate points and points smaller/greater than the repr. RAN
 * - ...
 */

namespace carl {

//////////////////
// Constructors //
//////////////////

template<typename Coeff>
ThomEncoding<Coeff>::ThomEncoding(
                std::shared_ptr<UnivariatePolynomial<Coeff>> ptr,
                const SignCondition& s
) {
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
        //CARL_LOG_INFO("carl.thom", "ptr = " << ptr << ", var = " << var << ", s = " << s << ", prev = " << prev);
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
        //CARL_LOG_INFO("carl.thom", "rational = " << rational << ", var = " << var);
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
        else if(n <= (unsigned)this->signs.size()) {
                return this->signs[n-1];
        }
        else if(n == (unsigned)this->signs.size() + 1) {
                //return Sign(sgn(p->derivative(mainVar).lcoeff()));
                MultivariatePolynomial<Coeff> lastDer = der_exp(this->polynomial(), this->mainVar, 0, this->polynomial().degree(this->mainVar)).back();
                return this->signOnPolynomial(lastDer);
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

template<typename Coeff>
SignCondition ThomEncoding<Coeff>::signsRealizedOn(const std::vector<UnivariatePolynomial<Coeff>>& list) const {
        static_assert(is_rational<Coeff>::value, "must provide a rational coefficient type here");
        std::vector<SignCondition> signConds;
        
        if(isMultiDimensional()) {
                std::vector<MultivariatePolynomial<Coeff>> deriv_list = this->accumulateDer();
                std::vector<MultivariatePolynomial<Coeff>> list_multiv;
                for(const auto& p : list) list_multiv.push_back(MultivariatePolynomial<Coeff>(p));
                deriv_list.insert(deriv_list.end(), list_multiv.begin(), list_multiv.end());
                signConds = signDetermination(deriv_list, this->accumulatePolynomials());
        }
        else {
                UnivariatePolynomial<Coeff> this_univ = this->polynomial().toUnivariatePolynomial();
                std::vector<UnivariatePolynomial<Coeff>> deriv_list = der_exp(this_univ, 1, this_univ.degree() - 1);
                deriv_list.insert(deriv_list.end(), list.begin(), list.end());
                signConds = signDetermination(deriv_list, this_univ);
        }
        
        SignCondition this_signs = this->accumulateSigns();
        SignCondition wanted; // linear search here...
        bool succes = false;
        for(const SignCondition& s : signConds) {
                if(isPrefix(this_signs, s)) {
                        wanted = s;
                        succes = true;
                        break;
                }
        }
        assert(succes);
        wanted.erase(wanted.begin(), wanted.begin() + this_signs.size());
        return wanted;
}

template<typename Coeff>
SignCondition ThomEncoding<Coeff>::signsRealizedOn(const std::vector<MultivariatePolynomial<Coeff>>& list) const {
        std::vector<SignCondition> signConds;
        
        if(isMultiDimensional()) {
                std::vector<MultivariatePolynomial<Coeff>> deriv_list = this->accumulateDer();
                std::vector<MultivariatePolynomial<Coeff>> list_multiv;
                for(const auto& p : list) list_multiv.push_back(p);
                deriv_list.insert(deriv_list.end(), list_multiv.begin(), list_multiv.end());
                signConds = signDetermination(deriv_list, this->accumulatePolynomials());
        }
        else {
                MultivariatePolynomial<Coeff> this_univ = this->polynomial();
                std::vector<MultivariatePolynomial<Coeff>> deriv_list = der_exp(this_univ, this->mainVar, 1, this_univ.degree(this->mainVar) - 1);
                deriv_list.insert(deriv_list.end(), list.begin(), list.end());
                signConds = signDetermination(deriv_list, this_univ);
        }
        
        SignCondition this_signs = this->accumulateSigns();
        SignCondition wanted; // linear search here...
        bool succes = false;
        for(const SignCondition& s : signConds) {
                if(isPrefix(this_signs, s)) {
                        wanted = s;
                        succes = true;
                        break;
                }
        }
        assert(succes);
        wanted.erase(wanted.begin(), wanted.begin() + this_signs.size());
        return wanted;
}


////////////////
// Comparison //
////////////////

template<typename C>
bool areComparable(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        if(lhs.isOneDimensional() && rhs.isOneDimensional() && lhs.getMainVar() == rhs.getMainVar()) {
                return true;
        }
        else if(lhs.isMultiDimensional() && rhs.isMultiDimensional()) {
                if(lhs.point->accumulatePolynomials() != rhs.point->accumulatePolynomials()) return false;
                if(lhs.point->accumulateSigns() != rhs.point->accumulateSigns()) return false;
                return true;
        }
        return false;
}

template<typename C>
ThomComparisonResult compareRational(const ThomEncoding<C>& t, const C& rat) {
        CARL_LOG_FUNC("carl.thom.compare", "t = " << t << " , rat = " << rat);
        if(t.isOneDimensional()) {
                UnivariatePolynomial<C> p_univ = t.polynomial().toUnivariatePolynomial();
                if(p_univ.cauchyBound() < carl::abs(rat)) {
                    assert(rat != 0);
                    std::cout << "cauchy bound: " << p_univ.cauchyBound() << std::endl;
                    std::cout << "rat: " << rat << std::endl;
                    if(rat > 0) return LESS;
                    else return GREATER;
                } 
                return compareThom(t, ThomEncoding<C>(rat, t.mainVar));
        }
        else {
                return compareThom(t, ThomEncoding<C>(rat, t.mainVar, t.point));
        }
}

// compares two RANs represented by thom encodings
template<typename C>
ThomComparisonResult compareThom(const ThomEncoding<C>& lhs, const ThomEncoding<C>& rhs) {
        CARL_LOG_FUNC("carl.thom.compare", "lhs = " << lhs << ", rhs = " << rhs);
        //CARL_LOG_ASSERT("carl.thom.compare", areComparable(lhs, rhs), "tried to compare\n" << lhs << "\n" << rhs);
        if(!areComparable(lhs, rhs)) {
                CARL_LOG_WARN("carl.thom.compare", "are not comparable: " << lhs << ", " << rhs << " ... returning LESS");
                return LESS;
        }
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
        CARL_LOG_FUNC("carl.thom.compare", "lhs = " << lhs << ", rhs = " << rhs);
        // assert that the main variables are the same ???
    
    
        
        // --- same underlying polynomials ---    
        if(lhs.polynomial() == rhs.polynomial()) {         
                return lhs.fullSignCondition() < rhs.fullSignCondition();
        }
        
        // --- different underlying polynomials ---
        else {
                CARL_LOG_TRACE("carl.thom.compare", "different underlying polynomiasl:\n" << lhs <<"\n" << rhs);
                UnivariatePolynomial<C> lhs_univ = lhs.polynomial().toUnivariatePolynomial();
                UnivariatePolynomial<C> rhs_univ = rhs.polynomial().toUnivariatePolynomial();
                uint deg_lhs = lhs_univ.degree();
                uint deg_rhs = rhs_univ.degree();
                std::vector<UnivariatePolynomial<C>> der_lhs_rhs = der_exp(lhs_univ, 1, deg_lhs - 1);
                std::vector<UnivariatePolynomial<C>> der_rhs = der_exp(rhs_univ, 0, deg_rhs);
                der_lhs_rhs.insert(der_lhs_rhs.end(), der_rhs.begin(), der_rhs.end());
                std::vector<SignCondition> signConds = signDetermination(der_lhs_rhs, lhs_univ); 
                SignCondition wanted; // linear search here...
                bool succes = false;
                for(const SignCondition& c : signConds) {
                        if(isPrefix(lhs.getSigns(), c)) {
                                wanted = c;
                                succes = true;
                                break;
                        }
                }
                assert(succes);
                wanted.erase(wanted.begin(), wanted.begin() + lhs.getSigns().size());

                ThomComparisonResult res = wanted < rhs.fullSignCondition();
                CARL_LOG_TRACE("carl.thom.compare", "result: " << res);
                return res;
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


////////////////////////////////////////////////
// Intermediate points and points above/below //
////////////////////////////////////////////////

template<typename Coeff>
ThomEncoding<Coeff> ThomEncoding<Coeff>::intermediateRolle(const ThomEncoding<Coeff>& lhs, const ThomEncoding<Coeff>& rhs) {
        CARL_LOG_FUNC("carl.thom.samples", lhs << ", " << rhs);
        assert(false);
        return ThomEncoding<Coeff>();
}

template<typename Coeff>
ThomEncoding<Coeff> ThomEncoding<Coeff>::intermediateBound(const ThomEncoding<Coeff>& lhs, const ThomEncoding<Coeff>& rhs) {       
        CARL_LOG_FUNC("carl.thom.samples", lhs << ", " << rhs);
        if(lhs.isOneDimensional()) {
                assert(rhs.isOneDimensional());
                UnivariatePolynomial<Coeff> prod_der = (lhs.polynomial() * rhs.polynomial()).toUnivariatePolynomial().derivative();
                Coeff epsilon;
                switch(SETTINGS.SEPERATION_BOUND) {
                        case SeperationBound::RUMP:
                                epsilon = rumpsBound(prod_der);
                                break;
                        default:
                                CARL_LOG_FATAL("carl.thom.samples", "bad settings");
                                epsilon = rumpsBound(prod_der);
                }                                            
                ThomEncoding<Coeff> newEncoding;
                // choose the encoding whose polynomial has smaller degree for the new encoding
                if(lhs.polynomial().degree(lhs.mainVar) <= rhs.polynomial().degree(rhs.mainVar)) {
                        newEncoding = lhs + epsilon;
                }
                else {
                        epsilon = -epsilon;
                        newEncoding = rhs + epsilon;
                }
                CARL_LOG_TRACE("carl.thom.samples", "result: " << newEncoding);
                return newEncoding;
        }
        else {
                return ThomEncoding<Coeff>::intermediateIterative(lhs, rhs);
        }
}

template<typename Coeff>
ThomEncoding<Coeff> ThomEncoding<Coeff>::intermediateIterative(const ThomEncoding<Coeff>& lhs, const ThomEncoding<Coeff>& rhs) {
        CARL_LOG_FUNC("carl.thom.samples", lhs << ", " << rhs);
        Coeff epsilon(SETTINGS.INITIAL_OFFSET);
        ThomEncoding<Coeff> res;
        // pick the polynomial with smaller degree here (or lhs, if equal degree)
        if(lhs.degree() <= rhs.degree()) {
                res = lhs + epsilon;
                while(res >= rhs) {
                        epsilon /= 2;
                        res = lhs + epsilon;
                }
        }
        else {
                epsilon = -epsilon;
                res = rhs + epsilon;
                while(lhs >= res) {
                        epsilon /= 2;
                        res = rhs + epsilon;
                }
        }
        CARL_LOG_TRACE("carl.thom.samples", "result: " << res);
        return res;
}

template<typename Coeff>
ThomEncoding<Coeff> ThomEncoding<Coeff>::intermediatePoint(const ThomEncoding<Coeff>& lhs, const ThomEncoding<Coeff>& rhs) {
        CARL_LOG_ASSERT("carl.thom.samples", areComparable(lhs, rhs), "intermediate points with non-comparable thom encodings called: " << lhs << ", " << rhs);
        CARL_LOG_ASSERT("carl.thom.samples", lhs < rhs, "intermediatePoint with wrong order or equal arguments called");
        switch(SETTINGS.INTERMEDIATE_POINT_ALGORITHM) {
                case IntermediatePointAlgorithm::ROLLE:
                        return ThomEncoding<Coeff>::intermediateRolle(lhs, rhs);
                case IntermediatePointAlgorithm::BOUND:
                        return ThomEncoding<Coeff>::intermediateBound(lhs, rhs);
                case IntermediatePointAlgorithm::ITERATIVE:
                        return ThomEncoding<Coeff>::intermediateIterative(lhs, rhs);
                default:
                        CARL_LOG_ERROR("carl.thom.samples", "bad settings");
                        return ThomEncoding<Coeff>::intermediateIterative(lhs, rhs);
        }
}

template<typename Coeff>
Coeff ThomEncoding<Coeff>::intermediatePoint(const ThomEncoding<Coeff>& lhs, const Coeff& rhs) {
        CARL_LOG_ASSERT("carl.thom.samples", lhs < rhs, "intermediatePoint with wrong order or equal arguments called");
        Coeff res;
        /*
        if(lhs.isOneDimensional()) {
                MultivariatePolynomial<Coeff> rhs_poly = lhs.mainVar - rhs;
                UnivariatePolynomial<Coeff> prod_der = (lhs.polynomial() * rhs_poly).toUnivariatePolynomial().derivative();
                Coeff epsilon = rumpsBound(prod_der);
                res = rhs - epsilon;     
        }
        else {
        */
                Coeff epsilon(1);
                res = rhs - epsilon;
                while(lhs >= res) {
                    epsilon /= 2;
                    res = rhs - epsilon;
                }
        //}
        CARL_LOG_TRACE("carl.thom.samples", "result: " << res);
        return res;
}

template<typename Coeff>
Coeff ThomEncoding<Coeff>::intermediatePoint(const Coeff& lhs, const ThomEncoding<Coeff>& rhs) {
        CARL_LOG_ASSERT("carl.thom.samples", lhs < rhs, "intermediatePoint with wrong order order in arguments called");
        Coeff res;
        /*
        if(rhs.isOneDimensional()) {
                MultivariatePolynomial<Coeff> lhs_poly = rhs.mainVar - lhs;
                UnivariatePolynomial<Coeff> prod_der = (rhs.polynomial() * lhs_poly).toUnivariatePolynomial().derivative();
                Coeff epsilon = rumpsBound(prod_der);
                res = lhs + epsilon;     
        }
        else {
        */
                Coeff epsilon(1);
                res = lhs + epsilon;
                while(res >= rhs) {
                    epsilon /= 2;
                    res = lhs + epsilon;
                }
        //}
        CARL_LOG_TRACE("carl.thom.samples", "result: " << res);
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

} // namespace carl

