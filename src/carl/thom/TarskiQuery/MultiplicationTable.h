/* 
 * File:   MultiplicationTable.h
 * Author: tobias
 *
 * Created on 1. Juni 2016, 11:14
 */

#pragma once

#include "../../groebner/groebner.h"
#include "../../core/MultivariatePolynomial.h"
#include "GroebnerBaseOps.h"

#include <algorithm>



namespace carl {


template<typename Coeff>
using MonomialBase = std::vector<_Monomial<Coeff>>;

// helper function that gives the list of coefficients of a polynomial already represented in that base
template<typename Coeff>
std::vector<Coeff> polynomialInBase(const MultivariatePolynomial<Coeff>& p, const MonomialBase<Coeff>& base) {
        CARL_LOG_ASSERT("carl.thom.tarski", base.size() >= p.size(), "p is not in <base>");
        // MAYBE OPTIMIZE THIS USING THAT THE MONOMIALS IN P AND IN BASE ARE ALREADY ORDERED
        MultivariatePolynomial<Coeff> p_prime(p);              
        std::vector<Term<Coeff>> terms = p_prime.getTerms();
        std::vector<Coeff> res(base.size(), Coeff(0));
        for(uint i = 0; i < base.size(); i++) {
                for(const auto& t : terms) {
                        if(t.monomial() == base[i].monomial()) {
                                res[i] = t.coeff();
                        }
                }
        }
        return res;
}

template<typename Coeff>
MultivariatePolynomial<Coeff> baseReprToPolynomial(const std::vector<Coeff>& repr, const MonomialBase<Coeff>& base) {
        CARL_LOG_ASSERT("carl.thom.tarski", repr.size() == base.size(), "");
        MultivariatePolynomial<Coeff> res(Coeff(0));
        for(uint i = 0; i < repr.size(); i++) {
                res += repr[i] * base[i];
        }
        return res;
}

template<typename Coeff>
std::vector<_Monomial<Coeff>> gatherMonomials(const MultivariatePolynomial<Coeff>& q) {
        std::vector<_Monomial<Coeff>> mons_q;
        for(const Term<Coeff> t : q) {
                mons_q.push_back(t * (Coeff(1) / t.coeff()));
        }
        return mons_q;
}

// algorithm 12.2
template<typename Coeff>
std::vector<Coeff> normalForm(const MultivariatePolynomial<Coeff>& p, const GB<Coeff>& gb) {
        CARL_LOG_FUNC("carl.thom.tarski", "p = " << p << ", gb = " << gb);
        
        MonomialBase<Coeff> Mon = mon(gb);
        MultivariatePolynomial<Coeff> q = p;
        std::vector<_Monomial<Coeff>> mons_q = gatherMonomials(q);
        // must sort to use std::includes
        std::sort(mons_q.begin(), mons_q.end());
        std::sort(Mon.begin(), Mon.end());      
        while(!std::includes(Mon.begin(), Mon.end(), mons_q.begin(), mons_q.end())) {
                // find greatest monomial in mons_q which is not in Mon
                for(const auto& x_alpha : mons_q) {
                        if(std::find(Mon.begin(), Mon.end(), x_alpha) == Mon.end()) {
                                for(const auto& G : gb) {
                                        _Monomial<Coeff> lmon_G = G.lterm();
                                        lmon_G = lmon_G * (Coeff(1) / lmon_G.coeff());
                                        _Monomial<Coeff> x_beta;                                       
                                        if(x_alpha.divide(lmon_G, x_beta)) {                                               
                                                // p.135
                                                Coeff cof_x_alpha(0);
                                                for(const auto& t : q) {
                                                        if(t.monomial() == x_alpha.monomial()) {
                                                                cof_x_alpha = t.coeff();
                                                        }
                                                }
                                                CARL_LOG_ASSERT("carl.thom.tarski", cof_x_alpha != 0, "");
                                                MultivariatePolynomial<Coeff> rhs = (cof_x_alpha / G.lcoeff()) * x_beta * G;
                                                q -= rhs;
                                                break;
                                        }     
                                }
                        break;       
                        }                      
                }
                mons_q = gatherMonomials(q);
                std::sort(mons_q.begin(), mons_q.end());
        }
        std::vector<Coeff> res = polynomialInBase(q, Mon);
        CARL_LOG_TRACE("carl.thom.tarski", "res = " << res);
        return res;
}




template<typename Coeff>
class MultiplicationTable {
        
public:
        typedef std::vector<Coeff> BaseRepr;
        typedef std::forward_list<std::pair<uint, uint>> IndexPairs;
        
private:
        std::map<_Monomial<Coeff>, BaseRepr> tab;
        MonomialBase<Coeff> base;
        
        struct TableContent {
                BaseRepr br;
                IndexPairs pairs;
        };
        
        // returns a list of all pairs of indicdes (i,j) such that base_i * base_j == c
        IndexPairs abc(const _Monomial<Coeff>& c) const {
                std::forward_list<std::pair<int, int>> res;
                for(uint i = 0; i < base.size(); i++) {
                        for(uint j = 0; j < base.size(); j++) {
                                if(base[i] * base[j] == c) {
                                        res.push_front(std::make_pair(i, j));
                                        if(i != j) res.push_front(std::make_pair(j, i));
                                }
                        }
                }
                return res;
        }
        
public:
        
        bool contains(const _Monomial<Coeff>& m) const {
                return tab.find(m) != tab.end();
        }
        
        // only call after initialization!
        BaseRepr& get(const _Monomial<Coeff>& m) {
                assert(contains(m));
                return tab[m];
        }
        
        BaseRepr get(const _Monomial<Coeff>& m) const {
                assert(contains(m));
                auto pair = *(tab.find(m));               
                return pair.second;
        }
        
        MonomialBase<Coeff> getBase() const {
                return base;
        }
        
        void init2(const GB<Coeff>& gb) {
                CARL_LOG_FUNC("carl.thom.tarski", "gb = " << gb);
                CARL_LOG_INEFFICIENT();
                MonomialBase<Coeff> Mon = mon(gb);
                std::sort(Mon.begin(), Mon.end());
                base = Mon;               
                std::vector<_Monomial<Coeff>> tabmon;
                for(const auto& m1 : Mon) {
                        for(const auto& m2 : Mon) {
                                _Monomial<Coeff> prod = m1 * m2;
                                if(std::find(tabmon.begin(), tabmon.end(), prod) == tabmon.end()) {
                                        tabmon.push_back(prod);
                                }                                
                        }
                }
                for(const auto& mon : tabmon) {
                        BaseRepr nf = normalForm(MultivariatePolynomial<Coeff>(mon), gb);
                        tab.insert(std::make_pair(mon, nf));                       
                }
        }
        
        // algorithm 12.3 (page 452)
        // computes normal forms of the elements in tab
        void init(const GB<Coeff>& gb) {
                CARL_LOG_FUNC("carl.thom.tarski", "gb = " << gb);
                
                // some needed values
                std::vector<_Monomial<Coeff>> Cor = cor(gb);
                std::vector<_Monomial<Coeff>> Bor = bor(gb);
                std::set<Variable> vars = gatherVariables(gb);
                
                // Mon is the monomial base of R[x_1,...x_n]/<gb>
                MonomialBase<Coeff> Mon = mon(gb);               
                std::sort(Mon.begin(), Mon.end());
                base = Mon;
                // monomials in bor in increasing order
                std::sort(Bor.begin(), Bor.end());
                
                // ---- step 0 ---- (not explicitly mentioned)
                // put BaseRepr of the monomials form Mon itself in table
                for(const auto& m : Mon) {
                        tab.insert(std::make_pair(m, polynomialInBase(MultivariatePolynomial<Coeff>(m), Mon)));
                }
                
                // ---- step 1 ----
                // compute the normal forms of the elements in Bor
                for(const auto& m : Bor) {
                        if(std::find(Cor.begin(), Cor.end(), m) != Cor.end()) {
                                // the monomial is also in Cor
                                // find the polynomial in the basis with the corresponding leading monomial
                                MultivariatePolynomial<Coeff> G;
                                for(const auto& p : gb) {
                                        if(p.lterm() == m) {
                                                G = p;
                                                break;
                                        }
                                }
                                MultivariatePolynomial<Coeff> diff = G.stripLT();
                                diff *= Coeff(-1);
                                tab.insert(std::make_pair(m, polynomialInBase(diff, Mon)));
                                CARL_LOG_TRACE("carl.thom.tarski", "tab = " << tab);                             
                        }
                        else {
                                // the monomial is not in core
                                // try to find a variable X_j, such that m / X_j is in Bor
                                CARL_LOG_TRACE("carl.thom.tarski", "not in cor: " << m);
                                auto it = vars.begin();
                                Variable var = *it;
                                _Monomial<Coeff> x_beta;
                                while(!m.divide(var, x_beta) || (std::find(Bor.begin(), Bor.end(), x_beta) == Bor.end())) {                                    
                                        it++;
                                        CARL_LOG_ASSERT("carl.thom.tarski", it != vars.end(), "");
                                        var = *it;
                                }
                                CARL_LOG_ASSERT("carl.thom.tarski", std::find(Bor.begin(), Bor.end(), x_beta) != Bor.end(), "");
                                CARL_LOG_TRACE("carl.thom.tarski", "x_beta = " << x_beta);
                                CARL_LOG_ASSERT("carl.thom.tarski", tab.find(x_beta) != tab.end(), "");
                                CARL_LOG_ASSERT("carl.thom.tarski", x_beta < m, "");
                                
                                BaseRepr nf_x_beta = tab[x_beta];
                                MultivariatePolynomial<Coeff> sum(Coeff(0));
                                
                                // sum over all pairs in Mon^2...
                                for(uint gamma = 0; gamma < Mon.size(); gamma++) {
                                        if(nf_x_beta[gamma] == Coeff(0)) {
                                                continue;
                                        }
                                        _Monomial<Coeff> x_gamma_prime = var * Mon[gamma];
                                        CARL_LOG_ASSERT("carl.thom.tarski", x_gamma_prime < m, "");
                                        CARL_LOG_ASSERT("carl.thom.tarski", tab.find(x_gamma_prime) != tab.end(), "");
                                        BaseRepr nf_x_gamma_prime = tab[x_gamma_prime];
                                        for(uint delta = 0; delta < Mon.size(); delta++) {
                                                Term<Coeff> prod(nf_x_beta[gamma]);
                                                prod = prod * nf_x_gamma_prime[delta];
                                                prod = prod * Mon[delta];
                                                sum += prod;
                                        }
                                }
                                tab.insert(std::make_pair(m, polynomialInBase(sum, Mon)));
                        }
                }
                
                // ---- step 2 ----
                // construct the matrices expressing multiplication by a variable (see step 2)
                
                // ---- step 3 ----
                // find the normal forms of all other elements in Tab(Mon)
                // Tab(Mon) = set of products of elements from Mon
                std::vector<_Monomial<Coeff>> tabmon;
                for(const auto& m1 : Mon) {
                        for(const auto& m2 : Mon) {
                                _Monomial<Coeff> prod = m1 * m2;
                                if(std::find(tabmon.begin(), tabmon.end(), prod) == tabmon.end()) {
                                        tabmon.push_back(prod);
                                }                                
                        }
                }
                std::sort(tabmon.begin(), tabmon.end());
                CARL_LOG_TRACE("carl.thom.tarski", "tabmon = " << tabmon);
                for(const auto& m : tabmon) {
                        if(tab.find(m) == tab.end()) {
                                // we do not have the normal form of m yet
                                CARL_LOG_TRACE("carl.thom.tarski", "still to compute: normal form for " << m);
                                // find variable x_j, s.t. m/x_j = x_beta and nf(x_beta) has already been computed
                                // almost the same as above, put that in a function...
                                auto it = vars.begin();
                                Variable var = *it;
                                _Monomial<Coeff> x_beta;
                                while(!m.divide(var, x_beta) || tab.find(x_beta) == tab.end()) {                                    
                                        it++;
                                        CARL_LOG_ASSERT("carl.thom.tarski", it != vars.end(), "");
                                        var = *it;
                                }
                                CARL_LOG_TRACE("carl.thom.tarski", "x_beta = " << x_beta);
                                CARL_LOG_TRACE("carl.thom.tarski", "var = " << var);
                                CARL_LOG_ASSERT("carl.thom.tarski", tab.find(x_beta) != tab.end(), "");
                                
                                // this is the matrix M_var
                                std::vector<BaseRepr> mat;
                                for(const auto& b : Mon) {
                                        CARL_LOG_ASSERT("carl.thom.tarski", tab.find(b * var) != tab.end(), "");
                                        mat.push_back(tab[b * var]);
                                }
                                BaseRepr nf_m(Mon.size(), Coeff(0)); // this is to compute
                                BaseRepr nf_x_beta = tab[x_beta];
                                // do the matrix-vector multiplication!
                                for(uint i = 0; i < Mon.size(); i++) {
                                        for(uint j = 0; j < Mon.size(); j++) {
                                                nf_m[i] += mat[j][i] * nf_x_beta[j];
                                        }
                                }
                                tab.insert(std::make_pair(m, nf_m));
                                
                        }
                }
        }
        
        
        //iterators
        typename std::map<_Monomial<Coeff>, BaseRepr>::iterator begin() {
                return tab.begin();
        }
        
        typename std::map<_Monomial<Coeff>, BaseRepr>::const_iterator begin() const {
                return tab.begin();
        }
        
        typename std::map<_Monomial<Coeff>, BaseRepr>::iterator end() {
                return tab.end();
        }
        
        typename std::map<_Monomial<Coeff>, BaseRepr>::const_iterator end() const {
                return tab.end();
        }
        
        template<typename C>
        friend std::ostream& operator<<(std::ostream& o, const MultiplicationTable<C>& table);
        
};

template<typename C>
std::ostream& operator<<(std::ostream& o, const MultiplicationTable<C>& table) {
        o << "Base = " << table.getBase() << std::endl;
        o << "Length = " << table.tab.size() << std::endl;
        for(const auto& pair : table.tab) {
                o << pair.first << "\t" << pair.second << std::endl;
        }
        return o;
}

        

       
}// namespace carl




