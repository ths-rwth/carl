/* 
 * File:   FactorizedPolynomial.h
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 10:55 AM
 */

#pragma once

#include <iostream>
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
        FactorizedPolynomialCache<P>&              mrCache;
        
    public:
            
        FactorizedPolynomial(); // no implementation
        FactorizedPolynomial( const P& _polynomial, FactorizedPolynomialCache<P>& );
        FactorizedPolynomial( const P& _polynomial, Factorization<P>&& _factorization, FactorizedPolynomialCache<P>& );
        FactorizedPolynomial( Factorization<P>&& _factorization, FactorizedPolynomialCache<P>& );
        FactorizedPolynomial( const FactorizedPolynomial<P>& );
        ~FactorizedPolynomial();
        
        FactorizedPolynomial<P>& operator=( const FactorizedPolynomial<P>& );
        
        template<typename P1>
        friend bool operator==(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
        template<typename P1>
        friend bool operator<(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
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
        
        size_t getHash() const
        {
            return mrCache.get( mCacheRef ).getHash();
        }
    };
    
} // namespace carl

namespace std
{
    template<typename P>
    struct hash<carl::FactorizedPolynomial<P>>
    {
        size_t operator()( const carl::FactorizedPolynomial<P>& _factPoly ) const 
        {
            return _factPoly.getHash();
        }
    };
} // namespace std

#include "FactorizedPolynomial.tpp"

