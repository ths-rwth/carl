/* 
 * File:   FactorizedPolynomial.h
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 10:55 AM
 */

#pragma once

#include <map>
#include <unordered_map>

namespace carl
{
    template<typename PolynomialType>
    class FactorizedPolynomial
    {
        friend class FactorizedPolynomialCache;
        
    private:
        // Members
        double mActivity;
        PolynomialType* mpPolynomial;
        std::map<PolynomialType*,size_t> mFactorization;
        FactorizedPolynomialCache& mrCache;
        
    public:
            
        FactorizedPolynomial(); // no implementation
        FactorizedPolynomial( double, PolynomialType*, std::map<PolynomialType*,size_t>& );
        FactorizedPolynomial( const FactorizedPolynomial& ); // no implementation
    };
    
} // namespace carl


#include "FactorizedPolynomial.tpp"

