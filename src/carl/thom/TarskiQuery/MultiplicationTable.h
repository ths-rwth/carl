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

// helper function that gives the list of coefficients of a polynomial in the base
template<typename Coeff>
std::vector<Coeff> polynomialInBase(const MultivariatePolynomial<Coeff>& p, const MonomialBase<Coeff>& base) {
        assert(base.size() >= p.size());
        MultivariatePolynomial<Coeff> p_prime(p);              

        std::vector<Term<Coeff>> terms = p_prime.getTerms();
        std::vector<Coeff> res(base.size(), Coeff(0));
        for(int i = 0; i < base.size(); i++) {
                for(const auto& t : terms) {
                        if(t.monomial() == base[i].monomial()) {
                                res[i] = t.coeff();
                        }
                }
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
        //std::cout << "p = " << p << std::endl;
        MonomialBase<Coeff> Mon = mon(gb);
        //std::cout << "Mon = " << Mon << std::endl;
        MultivariatePolynomial<Coeff> q = p;
        std::vector<_Monomial<Coeff>> mons_q = gatherMonomials(q);
        // must sort to use std::includes
        std::sort(mons_q.begin(), mons_q.end());
        std::sort(Mon.begin(), Mon.end());
        //std::cout << "mons_q = " << mons_q << std::endl;
        
        while(!std::includes(Mon.begin(), Mon.end(), mons_q.begin(), mons_q.end())) {
                // find greatest monomial in mons_q which is not in Mon
                for(const auto& x_alpha : mons_q) {
                        if(std::find(Mon.begin(), Mon.end(), x_alpha) == Mon.end()) {
                                //std::cout << "x_alpha = " << x_alpha << std::endl;
                                for(const auto& G : gb) {
                                        _Monomial<Coeff> lmon_G = G.lterm();
                                        lmon_G = lmon_G * (Coeff(1) / lmon_G.coeff());
                                        _Monomial<Coeff> x_beta;
                                        
                                        if(x_alpha.divide(lmon_G, x_beta)) {                                               
                                                //std::cout << "lmon_G = " << lmon_G << std::endl;
                                                //std::cout << "x_beta = " << x_beta << std::endl;
                                                // p.135
                                                Coeff cof_x_alpha(0);
                                                for(const auto& t : q) {
                                                        if(t.monomial() == x_alpha.monomial()) {
                                                                cof_x_alpha = t.coeff();
                                                                //std::cout << "cof_x_alpha = " << cof_x_alpha << std::endl;
                                                        }
                                                }
                                                assert(cof_x_alpha != 0);
                                                MultivariatePolynomial<Coeff> rhs = (cof_x_alpha / G.lcoeff()) * x_beta * G;
                                                q -= rhs;
                                                //std::cout << "updated q: " << q << std::endl;
                                                break;
                                        }     
                                }
                        break;       
                        }
                        
                }
                mons_q = gatherMonomials(q);
                //std::cout << "updated mons_q: " << mons_q << std::endl;
                std::sort(mons_q.begin(), mons_q.end());
        }
        std::vector<Coeff> res = polynomialInBase(q, Mon);
        //std::cout << "res = " << res << std::endl;
        return res;
}




template<typename Coeff>
class MultiplicationTable {
        
public:
        typedef std::vector<Coeff> BaseRepr;
        
private:
        std::map<_Monomial<Coeff>, BaseRepr> tab;
        MonomialBase<Coeff> base;
        
public:
        
        
        
        // only call after initialization!
        BaseRepr& get(const _Monomial<Coeff>& m) {
                assert(tab.find(m) != tab.end());
                return tab[m];
        }
        
        BaseRepr get(const _Monomial<Coeff>& m) const {
                assert(tab.find(m) != tab.end());
                auto pair = *(tab.find(m));               
                return pair.second;
        }
        
        MonomialBase<Coeff> getBase() const {
                return base;
        }
        
        void init2(const GB<Coeff>& gb) {
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
                //std::reverse(Bor.begin(), Bor.end());
                std::cout << "sorted border: " << Bor << std::endl;
                
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
                                std::cout << "G = " << G << std::endl;
                                std::cout << "m = " << m << std::endl;
                                //MultivariatePolynomial<Coeff> diff = G - m;
                                MultivariatePolynomial<Coeff> diff = G.stripLT();
                                diff *= Coeff(-1);
                                tab.insert(std::make_pair(m, polynomialInBase(diff, Mon)));
                                std::cout << tab << std::endl;
                                
                        }
                        else {
                                // the monomial is not in core
                                // try to find a variable X_j, such that m / X_j is in Bor
                                std::cout << "not in cor: " << m << std::endl;
                                auto it = vars.begin();
                                Variable var = *it;
                                _Monomial<Coeff> x_beta;
                                while(!m.divide(var, x_beta) || (std::find(Bor.begin(), Bor.end(), x_beta) == Bor.end())) {                                    
                                        it++;
                                        assert(it != vars.end());
                                        var = *it;
                                }
                                assert(std::find(Bor.begin(), Bor.end(), x_beta) != Bor.end());
                                std::cout << "x_beta = " << x_beta << std::endl;
                                assert(tab.find(x_beta) != tab.end());
                                assert(x_beta < m);
                                
                                BaseRepr nf_x_beta = tab[x_beta];
                                MultivariatePolynomial<Coeff> sum(Coeff(0));
                                
                                // sum over all pairs in Mon^2...
                                for(int gamma = 0; gamma < Mon.size(); gamma++) {
                                        if(nf_x_beta[gamma] == Coeff(0)) {
                                                continue;
                                        }
                                        _Monomial<Coeff> x_gamma_prime = var * Mon[gamma];
                                        
                                        std::cout << "x_gamma_prime =  " << x_gamma_prime << std::endl;
                                        assert(x_gamma_prime < m);
                                        assert(tab.find(x_gamma_prime) != tab.end());
                                        BaseRepr nf_x_gamma_prime = tab[x_gamma_prime];
                                        
                                        for(int delta = 0; delta < Mon.size(); delta++) {
                                                Term<Coeff> prod(nf_x_beta[gamma]);
                                                prod = prod * nf_x_gamma_prime[delta];
                                                prod = prod * Mon[delta];
                                                sum += prod;
                                        }
                                }
                               
                                
                                std::cout << polynomialInBase(sum, Mon) << std::endl;
                                tab.insert(std::make_pair(m, polynomialInBase(sum, Mon)));
                        }
                }
                // ---- step 2 ----
                // construct the matrices expressing multiplication by a variable
                
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
                std::cout << "tabmon = " << tabmon << std::endl;
                for(const auto& m : tabmon) {
                        if(tab.find(m) == tab.end()) {
                                // we do not have the normal form of m yet
                                std::cout << "still to compute: normal form for " << m << std::endl;
                                // find variable x_j, s.t. m/x_j = x_beta and nf(x_beta) has already been computed
                                // almost the same as above, put that in a function...
                                auto it = vars.begin();
                                Variable var = *it;
                                _Monomial<Coeff> x_beta;
                                while(!m.divide(var, x_beta) || tab.find(x_beta) == tab.end()) {                                    
                                        it++;
                                        assert(it != vars.end());
                                        var = *it;
                                }
                                std::cout << "x_beta = " << x_beta << std::endl;
                                std::cout << "var = " << var << std::endl;
                                assert(tab.find(x_beta) != tab.end());
                                
                                // this is the matrix M_var
                                std::vector<BaseRepr> mat;
                                for(const auto& b : Mon) {
                                        assert(tab.find(b * var) != tab.end());
                                        mat.push_back(tab[b * var]);
                                }
                                BaseRepr nf_m(Mon.size(), Coeff(0)); // this is to compute
                                BaseRepr nf_x_beta = tab[x_beta];
                                // do the matrix-vector multiplication!
                                for(int i = 0; i < Mon.size(); i++) {
                                        for(int j = 0; j < Mon.size(); j++) {
                                                nf_m[i] += mat[j][i] * nf_x_beta[j];
                                        }
                                }
                                std::cout << "nf_m = " << nf_m << std::endl;
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




