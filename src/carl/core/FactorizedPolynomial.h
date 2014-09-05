/* 
 * File:   FactorizedPolynomial.h
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 10:55 AM
 */

#pragma once

#include "FactorizedPolynomialCache.h"

namespace carl
{
    template<typename P>
    class FactorizedPolynomialCache;
    
    template<typename P>
    class FactorizedPolynomial
    {
    private:
        // Members
        typename FactorizedPolynomialCache<P>::Ref mCacheRef;
        FactorizedPolynomialCache<P>&     mrCache;
        
    public:
            
        FactorizedPolynomial(); // no implementation
        FactorizedPolynomial( typename FactorizedPolynomialCache<P>::Ref, FactorizedPolynomialCache<P>& );
        FactorizedPolynomial( const FactorizedPolynomial<P>& );
        ~FactorizedPolynomial();
        
        template<typename P1>
        friend bool operator==(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
        template<typename P1>
        friend const FactorizedPolynomial<P1> operator+(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
        template<typename P1>
        friend const FactorizedPolynomial<P1> operator-(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
        template<typename P1>
        friend const FactorizedPolynomial<P1> operator*(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
        template<typename P1>
        friend const FactorizedPolynomial<P1> operator/(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
        template<typename P1>
        friend const FactorizedPolynomial<P1> commonDivisor(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
        template<typename P1>
        friend const FactorizedPolynomial<P1> commonMultiple(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
        /**
         * @param _fpolyA A factorized polynomial.
         * @param _fpolyB 
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> gcd(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
        /**
         * Prints the factorization representation of the given factorized polynomial on the given output stream.
         * @param _out The stream to print on.
         * @param _fpoly The factorized polynomial to print.
         */
        template <typename P1>
        friend std::ostream& operator<<(std::ostream& _out, const FactorizedPolynomial<P1>& _fpoly);
    };
    
} // namespace carl


#include "FactorizedPolynomial.tpp"

