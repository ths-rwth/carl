/* 
 * File:   GroebnerBaseOps.tpp
 * Author: tobias
 *
 * Created on 8. Juni 2016, 08:34
 */

#pragma once

// -------------------------------------------------
// implementation of cor, bor and mon (see page 449)
// -------------------------------------------------

namespace carl {


template<typename Coeff>
using _Monomial = Term<Coeff>;

template<typename Coeff>
using GB = std::vector<MultivariatePolynomial<Coeff>>;

        
// cor (the corners of the staircase) are just the leading monomials of the polynomials in the groebner base
template<typename Coeff>
std::vector<carl::_Monomial<Coeff>> cor(const GB<Coeff>& g) {
        std::vector<_Monomial<Coeff>> res;
        for(const auto& p : g) {
                res.push_back(_Monomial<Coeff>(p.lmon()));
        }
        return res;
}

template<typename Coeff>
std::set<Variable> gatherVariables(const GB<Coeff>& g) {
        std::set<Variable> vars;
        for(const auto& p : g) {
                p.gatherVariables(vars);
        }
        return vars;
}

// Checks if the set mon of this groebner basis is finite (Prop. 12.7, page 541)
// Here we assume that the underlying ring is the polynomial ring in exactly the variables that actually occur in the basis.
// Otherwise, this should return false anyway.
// This is also a necessary condition for the system (for which g was computed) to be 0-dim.
template<typename Coeff>
bool hasFiniteMon(const GB<Coeff>& g) {
        std::set<Variable> vars = gatherVariables(g);
        std::vector<_Monomial<Coeff>> lmons = cor(g);
        for(const auto& v : vars) {
                for(const auto& m : lmons) {
                        assert(isOne(m.coeff()));
                        if(m.hasNoOtherVariable(v)) {
                                vars.erase(v);
                                break;
                        }
                }
        }
        return vars.empty();
}


// returns true iff any of the monomials in list divides m
template<typename Coeff>
bool anyDivides(const std::vector<_Monomial<Coeff>>& list, const _Monomial<Coeff>& m) {
        assert(isOne(m.coeff()));
        for(const auto& m_prime : list) {
                assert(isOne(m_prime.coeff()));
                if(m.divisible(m_prime)) {
                        return true;
                }
        }
        return false;
}
        

// mon are the monomials below the staircase
// this functions will assert that the result is finite
// currently only works for 2 variables
template<typename Coeff>
std::vector<_Monomial<Coeff>> mon(const GB<Coeff>& g) {
        assert(hasFiniteMon(g));      
        std::vector<_Monomial<Coeff>> res;
        std::vector<_Monomial<Coeff>> lmons = cor(g);
        std::set<Variable> vars = gatherVariables(g);
        assert(vars.size() == 2);
        Variable v1 = *vars.begin();
        Variable v2 = *(++vars.begin());
        _Monomial<Coeff> currMon(Coeff(1)); // we use polynomials here instead of monomials because they cant be 0
        while(!anyDivides(lmons, currMon)) {
                while(!anyDivides(lmons, currMon)) {
                        res.push_back(currMon);
                        currMon = currMon * v2;
                }
                // remove v2 from the current monomial
                std::set<Variable> currVars;
                currMon.gatherVariables(currVars);
                if(currVars.find(v2) != currVars.end()) {
                        currMon = _Monomial<Coeff>(currMon.monomial()->dropVariable(v2));
                }
                // finally augment the degree of v1 in the current monomial
                currMon = currMon * v1;
        }
        
        
        return res;
}


// border of the staircase - can be easily computed using the set mon(g)
template<typename Coeff>
std::vector<_Monomial<Coeff>> bor(const GB<Coeff>& g) {
        std::vector<_Monomial<Coeff>> res;
        std::vector<_Monomial<Coeff>> lmons = cor(g);
        std::set<Variable> vars = gatherVariables(g);
        std::vector<_Monomial<Coeff>> Mon = mon(g);
        for(const Variable& v : vars) {
                for(const _Monomial<Coeff>& m : Mon) {
                        assert(isOne(m.coeff()));
                        _Monomial<Coeff> currMon = m * v;
                        if(std::find(res.begin(), res.end(), currMon) == res.end()) { // not already contained
                                if(std::find(Mon.begin(), Mon.end(), MultivariatePolynomial<Coeff>(currMon)) == Mon.end()) { // not contained in Mon
                                        res.push_back(currMon);
                                }
                        }
                }
        }
        return res;
        
}

} // namespace carl

