/* 
 * File:   TarskiQuery.h
 * Author: tobias
 *
 * Created on 14. Juni 2016, 19:19
 */

#pragma once

#include <memory>

#include "MultivariateTarskiQuery.h"
#include "UnivariateTarskiQuery.h"

#include "../ThomSettings.h"

namespace carl {


template<typename Coeff>
bool isZeroDimensionalSystem(const std::vector<UnivariatePolynomial<Coeff>>& system) {
        assert(system.size() == 1);
        return !system.front().isZero();
}

template<typename Coeff>
bool isZeroDimensionalSystem(const std::vector<MultivariatePolynomial<Coeff>>& system) {
        return true;
}


/*
 * A TarskiQueryManager is an object associated to a zero set (represented implicitly by a polynomial set).
 * Its purpose is to compute queries on the same set more efficiently.
 * This is mainly interesting for the multivariate case. The multiplication table
 * will only be computed once.
 * 
 * Yet we want to have an interface for both univ. and multiv. polynomials.
 * 
 */
// http://stackoverflow.com/questions/17056579/template-specialization-and-inheritance
template<typename Polynomial, typename Settings = ThomDefaultSettings>
class CommonTarskiQueryManager {
        
public:
        typedef int QueryResultType;
        typedef std::vector<Polynomial> ZeroSet;
        
        /*
         * Computes the tarski query for the given polynomial over the zero set this manager is associated to
         */
        virtual QueryResultType operator()(const Polynomial&) const = 0;
        
        /*
         * This function takes two polynomials and returns their product reduced by
         * the ideal that is defined by the zero set of this manager.
         * This is used in signDetermination to keep products small.
         */
        virtual Polynomial reduceProduct(const Polynomial&, const Polynomial&) const = 0;
};

template<typename Polynomial, typename Settings = ThomDefaultSettings>
class TarskiQueryManager : public CommonTarskiQueryManager<Polynomial, Settings> {};



/*
 * univariate specialization of the tarski query manager
 * safes the derivative so that is does not have to be computed multiple times
 */
template<typename C, typename Settings>
class TarskiQueryManager<UnivariatePolynomial<C>, Settings> : public CommonTarskiQueryManager<UnivariatePolynomial<C>, Settings> {
        
private:
        UnivariatePolynomial<C> der_z;
        UnivariatePolynomial<C> z; 
        
        
public:
        TarskiQueryManager(const std::vector<UnivariatePolynomial<C>>& zeroSet) : der_z(zeroSet.front().derivative()), z(zeroSet.front()) {
                static_assert(is_rational<C>::value, "must setup the univariate tarski query manager on univariate rational polynomials");
                CARL_LOG_ASSERT("carl.thom.tarski", zeroSet.size() == 1, "the univariate taq query accepts only one polynomial as zero set");
                //assert(isZeroDimensionalSystem(z));
        }
        
        typename CommonTarskiQueryManager<UnivariatePolynomial<C>>::QueryResultType operator()(const UnivariatePolynomial<C>& p) const {
                return univariateTarskiQuery(p, z, der_z);
        }
        
        // query on the constant polynomial n
        typename CommonTarskiQueryManager<UnivariatePolynomial<C>>::QueryResultType operator()(const C n) const {
                UnivariatePolynomial<C> constantPol = n * z.one();
                return (*this)(constantPol);
        }
        
        UnivariatePolynomial<C> reduceProduct(const UnivariatePolynomial<C>& a, const UnivariatePolynomial<C>& b) const {
                return a.remainder(z) * b.remainder(z);
        }
};




/*
 * multivariate specialization of the tarski query manager
 * safes the multiplication table and the groebner base
 */
template<typename C, typename Settings>
class TarskiQueryManager<MultivariatePolynomial<C>, Settings> : public CommonTarskiQueryManager<MultivariatePolynomial<C>, Settings> {
        
private:
        /*
         * In the sign determination algorithm the multivariate tarski queries
         * are always computed w.r.t. the same set of zeros.
         * We therefore cache the multiplication table and the groebner base.
         */
        MultiplicationTable<C> tab;
        GB<C> gb;
        
public:
        TarskiQueryManager(const std::vector<MultivariatePolynomial<C>>& zeroSet) {
                static_assert(is_rational<C>::value, "must setup the multivariate tarski query manager on multiv. rational polynomials");
#if defined LOGGING_CARL
                if(gatherVariables(zeroSet).size() == 1) {
                        CARL_LOG_WARN("carl.thom.tarski", "zero set with just 1 variable in multivariate taq manager: " << zeroSet);
                }
#endif
                // set up the groebner base
                GBProcedure<MultivariatePolynomial<C>, Buchberger, StdAdding> gbobject;
                for(const auto& p : zeroSet) gbobject.addPolynomial(p);
                gbobject.reduceInput();
                gbobject.calculate();
                gb = gbobject.getIdeal().getGenerators();
                CARL_LOG_ASSERT("carl.thom.tarski", hasFiniteMon(gb), "tried to setup a tarski query manager on an infinite zero set");
                CARL_LOG_INFO("carl.thom.tarski", "size of mon is " << mon(gb).size());
                // set up the multiplication table
                if(Settings::MULT_TABLE_USE_NF_ALG) {
                        tab.init2(gb);
                }
                else {
                        tab.init(gb);
                }
        }
        
        typename CommonTarskiQueryManager<MultivariatePolynomial<C>>::QueryResultType operator()(const MultivariatePolynomial<C>& p) const {
                return multivariateTarskiQuery(p, tab, gb);
        }
        
        // query on the constant polynomial n
        typename CommonTarskiQueryManager<MultivariatePolynomial<C>>::QueryResultType operator()(const C n) const {
                MultivariatePolynomial<C> constantPol(n);
                return (*this)(constantPol);
        }
        
        MultivariatePolynomial<C> reduceProduct(const MultivariatePolynomial<C>& a, const MultivariatePolynomial<C>& b) const {
                //return baseReprToPolynomial(multiply(normalForm(a, gb), normalForm(b, gb), tab), tab.getBase());
                CARL_LOG_WARN("carl.thom.tarski", "in theory we should use 'multiply' here!");
                return baseReprToPolynomial(normalForm(a * b, gb), tab.getBase());
        }
};

/*
 * Use a tarski query manager rather then these methods if multiple queries on the same zero set are to be computed
 * 
 */
template<typename Polynomial>
int tarskiQuery(const Polynomial& p, const std::vector<Polynomial>& zeroSet) {
        TarskiQueryManager<Polynomial> taq(zeroSet);
        return taq(p);
}

template<typename Polynomial>
int tarskiQuery(const Polynomial& p, const Polynomial& zeroSet) {
        return tarskiQuery(p, std::vector<Polynomial>(1, zeroSet));
}


} // namespace carl

