/* 
 * File:   FactorizedPolynomial.h
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 10:55 AM
 */

#pragma once

#include <iostream>
#include "carl/util/Cache.h"
#include "PolynomialFactorizationPair.h"

namespace carl
{   
    template<typename P>
    class FactorizedPolynomial
    {
    private:
        // Members
        /// The reference of the entry in the cache corresponding to this factorized polynomial.
        typename Cache<PolynomialFactorizationPair<P>>::Ref mCacheRef;
        /// The cache in which the actual content of this factorized polynomial is stored.
        Cache<PolynomialFactorizationPair<P>>&              mrCache;
        
        /**
         * Updates the hash of the entry in the cache corresponding to this factorized 
         * polynomial, which is also its hash.
         */
        void rehash()
        {
            mrCache.rehash( mCacheRef );
        }
        
        void strengthenActivity() const
        {
            mrCache.strengthenActivity( mCacheRef );
        }
        
    public:
           
        // Constructors.
        FactorizedPolynomial(); // no implementation
        FactorizedPolynomial( const P& _polynomial, Cache<PolynomialFactorizationPair<P>>& );
        FactorizedPolynomial( const P& _polynomial, Factorization<P>&& _factorization, Cache<PolynomialFactorizationPair<P>>& );
        FactorizedPolynomial( Factorization<P>&& _factorization, Cache<PolynomialFactorizationPair<P>>& );
        FactorizedPolynomial( const FactorizedPolynomial<P>& );
        
        // Destructor.
        ~FactorizedPolynomial();
        
        /**
         * Copies the given factorized polynomial.
         * @param The factorized polynomial to copy.
         * @return A reference to the copy of the given factorized polynomial.
         */
        FactorizedPolynomial<P>& operator=( const FactorizedPolynomial<P>& );
        
        /**
         * @return The reference of the entry in the cache corresponding to this factorized polynomial.
         */
        typename Cache<PolynomialFactorizationPair<P>>::Ref cacheRef() const
        {
            return mCacheRef;
        }
        
        /**
         * @return The cache used by this factorized polynomial.
         */
        const Cache<PolynomialFactorizationPair<P>>& cache() const
        {
            return mrCache;
        }
        
        /**
         * @return The entry in the cache corresponding to this factorized polynomial.
         */
        const PolynomialFactorizationPair<P>& content() const
        {
            return mrCache.get( mCacheRef );
        }
        
        /**
         * @return The hash value of the entry in the cache corresponding to this factorized polynomial.
         */
        size_t getHash() const
        {
            return mrCache.get( mCacheRef ).getHash();
        }

        /**
         * @param _fpolyA The first summand.
         * @param _fpolyB The second summand.
         * @return The sum of the two given factorized polynomials.
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> operator+(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);

        /**
         * @param _fpolyA The minuend.
         * @param _fpolyB The subtrahend.
         * @return The difference between the two given factorized polynomials.
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> operator-(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
        /**
         * @param _fpolyA The first factor.
         * @param _fpolyB The second factor.
         * @return The product of the two given factorized polynomials.
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> operator*( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB );

        /**
         * @param _fpolyA The factorized polynomial to divide by the common factors of the two given factorized polynomials.
         * @param _fpolyB The factorized polynomial, whose common factors with the first given factorized polynomial are divided from it.
         * @return The quotient of the first given factorized polynomial and the common factors of both given factorized polynomials.
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> lazyDiv( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB );

        /**
         * @param _fpolyA The first factorized polynomial to compute the common divisor for.
         * @param _fpolyB The second factorized polynomial to compute the common divisor for.
         * @return A common divisor of the two given factorized polynomials.
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> commonDivisor( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB );

        /**
         * @param _fpolyA The first factorized polynomial to compute the common multiple for.
         * @param _fpolyB The second factorized polynomial to compute the common multiple for.
         * @return A common multiple of the two given factorized polynomials.
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> commonMultiple( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB );

        /**
         * Determines the greatest common divisor of the two given factorized polynomials. The method exploits the partial factorization
         * stored in the arguments and refines it. (c.f. Accelerating Parametric Probabilistic Verification, Section 4)
         * @param _fpolyA The first factorized polynomial to compute the greatest common divisor for.
         * @param _fpolyB The second factorized polynomial to compute the greatest common divisor for.
         * @return The greatest common divisor of the two given factorized polynomials.
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> gcd(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
    };
    
    /**
     * @param _fpolyA The first factorized polynomial to compare.
     * @param _fpolyB The second factorized polynomial to compare.
     * @return true, if the two given factorized polynomials are equal.
     */
    template<typename P>
    bool operator==(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB);
    
    /**
     * @param _fpolyA The first factorized polynomial to compare.
     * @param _fpolyB The second factorized polynomial to compare.
     * @return true, if the first given factorized polynomial is less than the second given factorized polynomial.
     */
    template<typename P>
    bool operator<(const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB);

    /**
     * Prints the factorization representation of the given factorized polynomial on the given output stream.
     * @param _out The stream to print on.
     * @param _fpoly The factorized polynomial to print.
     * @return The output stream after inserting the output.
     */
    template <typename P>
    std::ostream& operator<<(std::ostream& _out, const FactorizedPolynomial<P>& _fpoly);
    
    
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

