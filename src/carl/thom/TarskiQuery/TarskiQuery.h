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
};




/*
 * multivariate specialization of the tarski query manager
 * safes the multiplication table and the groebner base
 */
template<typename C, typename Settings>
class TarskiQueryManager<MultivariatePolynomial<C>, Settings> : public CommonTarskiQueryManager<MultivariatePolynomial<C>, Settings> {
        
private:
        MultiplicationTable<C> tab;
        GB<C> gb;
        
public:
        TarskiQueryManager(const std::vector<MultivariatePolynomial<C>>& zeroSet) {
                // set up the groebner base
                GBProcedure<MultivariatePolynomial<C>, Buchberger, StdAdding> gbobject;
                for(const auto& p : zeroSet) gbobject.addPolynomial(p);
                gbobject.reduceInput();
                gbobject.calculate();
                gb = gbobject.getIdeal().getGenerators();
                assert(hasFiniteMon(gb));
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

#include "TarskiQuery.tpp"
