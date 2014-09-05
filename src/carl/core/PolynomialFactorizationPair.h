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
    class FactorizedPolynomialCache;
    
    template<typename P>
    using Factorization = std::map<FactorizedPolynomial<P>, size_t>;
    
    template<typename P>
    class PolynomialFactorizationPair
    {
        template<typename P1>
        friend class FactorizedPolynomialCache;
        
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
        
        template<typename P2>
        friend bool operator==(const PolynomialFactorizationPair<P2>& _polyFactA, const PolynomialFactorizationPair<P2>& _polyFactB);
        
        template<typename P2>
        friend bool operator<(const PolynomialFactorizationPair<P2>& _polyFactA, const PolynomialFactorizationPair<P2>& _polyFactB);
        
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