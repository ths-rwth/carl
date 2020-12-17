/* 
 * File:   TarskiQueryManager.h
 * Author: tobias
 *
 * Created on 27. August 2016, 15:58
 */

#pragma once

#include <iterator>

#include "MultiplicationTable.h"
#include "MultivariateTarskiQuery.h"
#include "UnivariateTarskiQuery.h"

#include <carl/core/polynomialfunctions/to_univariate_polynomial.h>

namespace carl {
        
/*
 * The Tarski query manager is a class designed to manage the computation of Tarski queries.
 * 
 */ 
template<typename Number>
class TarskiQueryManager {

public:
        using QueryResultType = int;
        
private:
        using Polynomial = MultivariatePolynomial<Number>;
        
        // for the univariate case
        UnivariatePolynomial<Number> mZ = UnivariatePolynomial<Number>(Variable::NO_VARIABLE);
        UnivariatePolynomial<Number> mDer = UnivariatePolynomial<Number>(Variable::NO_VARIABLE);
        
        // for the multivariate case
        MultiplicationTable<Number> mTab;
        bool mTrivialGb = false;
        
        mutable std::map<Polynomial, QueryResultType> mCache;
        
public:
        TarskiQueryManager() = default;
        
        template<typename InputIt>
        TarskiQueryManager(InputIt first, InputIt last) {
                CARL_LOG_TRACE("carl.thom.tarski.manager", "setting up a taq manager on " << std::vector<Polynomial>(first, last));
                // univariate manager
                if(std::distance(first, last) == 1 && first->isUnivariate()) {
                        CARL_LOG_TRACE("carl.thom.tarski.manager", "as a UNIVARIATE manager");
                        mZ = carl::to_univariate_polynomial(*first);
                        CARL_LOG_ASSERT("carl.thom.tarski.manager", !carl::isZero(mZ), "");
                        mDer = derivative(mZ);
                        CARL_LOG_ASSERT("carl.thom.tarski.manager", this->isUnivariateManager(), "");
                }
                // multivariate manager
                else {
                        CARL_LOG_TRACE("carl.thom.tarski.manager", "as a MULTIVARIATE manager");
                        GroebnerBase<Number> gb(first, last);
                        if(gb.isTrivialBase()) {
                                mTrivialGb = true;
                        }
                        else {
                                CARL_LOG_ASSERT("carl.thom.tarski.manager", gb.hasFiniteMon(), "");
                                if(!gb.hasFiniteMon()) {
                                        std::cout << "aborting because it was tried to set up a tarki query manager on a non zero-dimensional zero set" << std::endl;
                                        std::exit(23);
                                }
                                mTab = MultiplicationTable<Number>(gb);
                        }
                        CARL_LOG_ASSERT("carl.thom.tarski.manager", !this->isUnivariateManager(), "");
                }
        }
        
        QueryResultType operator()(const Polynomial& p) const {
                CARL_LOG_TRACE("carl.thom.tarski.manager", "computing taq on " << p << " ... ");
                if(carl::isZero(p)) return 0;
                QueryResultType res;
                
                // return cached query result
                if(getCached(p, res)) {
                        CARL_LOG_TRACE("carl.thom.tarski.manager", "found in cache: " << res);
                        return res;
                }
                
                // univariate manager
                if(this->isUnivariateManager()) {
                        CARL_LOG_ASSERT("carl.thom.tarski.manager", p.isUnivariate(), "");
                        UnivariatePolynomial<Number> pUniv(Variable::NO_VARIABLE);
                        if(p.isConstant()) pUniv = UnivariatePolynomial<Number>(mZ.mainVar(), p.lcoeff());
                        else pUniv = carl::to_univariate_polynomial(p);
                        CARL_LOG_ASSERT("carl.thom.tarski.manager", pUniv.mainVar() == mZ.mainVar(),
                                "cannot compute tarski query of " << p << " on " << mZ);
                        res = univariateTarskiQuery(pUniv, mZ, mDer);
                }
                
                // multivariate manager
                else {
                        if(mTrivialGb) res = 0;
                        else {
                        // todo: check if variables in p are also in the polynomials defining the zero set
                                res = multivariateTarskiQuery(p, mTab);
                        }
                }
                cache(p, res);
                CARL_LOG_TRACE("carl.thom.tarski.manager", res);
                return res;
        }
        
        QueryResultType operator()(const Number& c) const {
                return (*this)(Polynomial(c));
        }
        
        Polynomial reduceProduct(const Polynomial& a, const Polynomial& b) const {
                if(this->isUnivariateManager()) {
                        // todo: implement
                        return a * b;
                }
                else {
                        return mTab.baseReprToPolynomial(mTab.reduce(a * b));
                }
                
        }
        

        
private:
        
        bool isUnivariateManager() const {
                return !carl::isZero(mZ);
        }
        
        /*
         * looks for the normalization of p in the cache
         */
        bool getCached(const Polynomial& p, QueryResultType& res) const {
                auto it = mCache.find(p.normalize());
                if(it != mCache.end()) {
						res = int(sgn(p.lcoeff())) * (it->second);
                        return true;
                }
                return false;
        }
        
        /*
         * writes normalized p with correspoding result in cache
         */
        void cache(const Polynomial& p, const QueryResultType res) const {
                mCache.insert(std::make_pair(p.normalize(), int(sgn(p.lcoeff())) * res));
        }
        
}; // class TarskiQueryManager

} // namespace carl
