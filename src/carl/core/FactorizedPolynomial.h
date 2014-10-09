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
    template <typename P>
    using Coeff = typename UnderlyingNumberType<P>::type;

    template<typename P>
    class FactorizedPolynomial
    {
    public:
        template<typename P1>
        friend Factorization<P1> gcd( const PolynomialFactorizationPair<P1>& _pfPairA, const PolynomialFactorizationPair<P1>& _pfPairB, Factorization<P1>& _restA, Factorization<P1>& _rest2B, bool& _pfPairARefined, bool& _pfPairBRefined );
        
        typedef Coeff<P> CoeffType;

    private:
        // Members

        /**
         * The reference of the entry in the cache corresponding to this factorized polynomial.
         */
        typename Cache<PolynomialFactorizationPair<P>>::Ref mCacheRef;

        /**
         * The cache in which the actual content of this factorized polynomial is stored.
         */
        Cache<PolynomialFactorizationPair<P>>& mrCache;

        /**
         * Co-prime coefficient of the factorization
         */
        mutable Coeff<P> mCoefficient;
        
        /**
         * Updates the hash of the entry in the cache corresponding to this factorized 
         * polynomial, which is also its hash.
         */
        void rehash() const
        {
            mrCache.rehash( mCacheRef );
        }
        
        void strengthenActivity() const
        {
            mrCache.strengthenActivity( mCacheRef );
        }
        
        /**
         * Getter
         * @return Coefficient of the polynomial.
         */
        const Coeff<P>& coefficient() const
        {
            return mCoefficient;
        }

        /**
         * Computes the common divisor with rest of two factorizations.
         * @param _fFactorizationA The factorization of the first polynomial.
         * @param _fFactorizationB The factorization of the second polynomial.
         * @param _fFactorizationRestA Returns the remaining factorization of the first polynomial without the common divisor
         * @param _fFactorizationRestB Returns the remaining factorization of the second polynomial without the common divisor
         * @return The factorization of a common divisor of the two given factorized polynomials.
         */
        template<typename P1>
        friend const Factorization<P1> commonDivisor( const FactorizedPolynomial<P1>& _fFactorizationA, const FactorizedPolynomial<P1>& _fFactorizationB, Factorization<P1>& _fFactorizationRestA, Factorization<P1>& _fFactorizationRestB);

        /**
         * @param _fpolyA The factorized polynomial to divide by the common factors of the two given factorized polynomials.
         * @param _fpolyB The factorized polynomial, whose common factors with the first given factorized polynomial are divided from it.
         * @return The quotient of the first given factorized polynomial and the common factors of both given factorized polynomials.
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> lazyDiv( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB );

    public:
           
        // Constructors.
        FactorizedPolynomial(); // no implementation
        FactorizedPolynomial( const P& _polynomial, Cache<PolynomialFactorizationPair<P>>& );
        FactorizedPolynomial( const P& _polynomial, Factorization<P>&& _factorization, Coeff<P>&, Cache<PolynomialFactorizationPair<P>>& );
        FactorizedPolynomial( Factorization<P>&& _factorization, Coeff<P>&, Cache<PolynomialFactorizationPair<P>>& );
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
         * @return The factorization of this polynomial.
         */
        const Factorization<P>& factorization() const
        {
            //TODO (matthias) activate?
            //content().flattenFactorization();
            return content().factorization();
        }

        /**
         * @return true, if the factorized polynomial is one.
         */
        bool isOne() const
        {
            return factorization().empty() && mCoefficient == 1;
        }

        /**
         * @return true, if the factorized polynomial is zero.
         */
        bool isZero() const
        {
            return factorization().empty() && mCoefficient == 0;
        }

        /**
         * @return true, if the factorized polynomial is constant.
         */
        bool isConstant() const
        {
            return factorization().empty();
        }
        
        CoeffType constantPart() const
        {
            CoeffType result( mCoefficient );
            result *= content().constantPart();
            return result;
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
         * @param _fpoly The factor to multiply this factorized polynomial with.
         * @return This factorized polynomial after multiplying it with the given factor.
         */
        FactorizedPolynomial<P>& operator*=( const FactorizedPolynomial<P>& _fpoly );

        /**
         * Calculates the quotient. Notice: the divisor has to be a factor of the polynomial.
         * @param _fdivisor The divisor
         * @return The quotient
         */
        const FactorizedPolynomial<P> quotient( const FactorizedPolynomial<P>& _fdivisor ) const;

        /**
         * Calculates the quotient of the polynomials. Notice: the second polynomial has to be a factor of the first polynomial.
         * @param _fpolyA The dividend.
         * @param _fpolyB The divisor.
         * @return The quotient
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> quotient( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB )
        {
            return _fpolyA.quotient( _fpolyB );
        }

        /**Computes the least common multiple of two given polynomials. The method refines the factorization.
         * @param _fpolyA The first factorized polynomial to compute the lcm for.
         * @param _fpolyB The second factorized polynomial to compute the lcm for.
         * @return The lcm of the two given factorized polynomials.
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> lcm( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB );

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
         * @param _fpolyRestA Returns the remaining part of the first factorized polynomial without the gcd.
         * @param _fpolyRestB Returns the remaining part of the second factorized polynomial without the gcd.
         * @return The greatest common divisor of the two given factorized polynomials.
         */
        template<typename P1>
        friend const FactorizedPolynomial<P1> gcd(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB, FactorizedPolynomial<P1>& _fpolyRestA, FactorizedPolynomial<P1>& _fpolyRestB);
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

