/* 
 * File:   PolynomialFactorizationPair.h
 * Author: Florian Corzilius
 *
 * Created on September 5, 2014, 3:57 PM
 */

#pragma once

#include <map>

namespace carl
{
    template<typename P>
    class FactorizedPolynomial;
    
    template<typename P>
    using Factorization = std::map<FactorizedPolynomial<P>, size_t>;
    
    template<typename P>
    class PolynomialFactorizationPair
    {
        
    private:
        // Members
        size_t           mHash;
        P*               mpPolynomial;
        Factorization<P> mFactorization;
        
    public:
        PolynomialFactorizationPair(); // no implementation
        PolynomialFactorizationPair( Factorization<P>&& _factorization, P* _polynomial = nullptr );
        PolynomialFactorizationPair( const PolynomialFactorizationPair& ); // no implementation
        
        void rehash();
        
        bool hasExpandedRepresentation() const
        {
            return mpPolynomial != nullptr;
        }
        
        const Factorization<P>& factorization() const
        {
            return mFactorization;
        }
        
        Factorization<P>& rFactorization()
        {
            return mFactorization;
        }
        
        size_t getHash() const
        {
            return mHash;
        }
        
        template<typename P1>
        friend bool operator==(const PolynomialFactorizationPair<P1>& _polyFactA, const PolynomialFactorizationPair<P1>& _polyFactB);
        
        template<typename P1>
        friend bool operator<(const PolynomialFactorizationPair<P1>& _polyFactA, const PolynomialFactorizationPair<P1>& _polyFactB);
        
        /**
         * Prints the given polynomial-factorization pair on the given output stream.
         * @param _out The stream to print on.
         * @param _pfPair The polynomial-factorization pair to print.
         * @return The output stream after inserting the output.
         */
        template <typename P1>
        friend std::ostream& operator<<(std::ostream& _out, const PolynomialFactorizationPair<P1>& _pfPair);
        
    private:
        const P* pPolynomial() const
        {
            return mpPolynomial;
        }
        
        void setPolynomial( P* _polynomial )
        {
            assert( _polynomial != nullptr );
            if( _polynomial != mpPolynomial )
            {
                assert( mpPolynomial != nullptr );
                mpPolynomial = _polynomial;
            }
        }
        
    };
} // namespace carl

namespace std
{
    template<typename P>
    struct hash<carl::PolynomialFactorizationPair<P>>
    {
        size_t operator()( const carl::PolynomialFactorizationPair<P>& _pfp ) const 
        {
            return _pfp.getHash();
        }
    };
} // namespace std

#include "PolynomialFactorizationPair.tpp"