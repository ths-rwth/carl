/* 
 * File:   PolynomialFactorizationPair.h
 * Author: Florian Corzilius
 *
 * Created on September 5, 2014, 3:57 PM
 */

#pragma once

#include <map>
#include <mutex>

#include <carl/poly/umvpoly/Monomial.h>
#include <carl/core/Common.h>

namespace carl
{
    template<typename P>
    class FactorizedPolynomial;
    
    template<typename P>
    class Factorization: public std::map<FactorizedPolynomial<P>, carl::exponent>
    {
        private:
            using super = std::map<FactorizedPolynomial<P>, carl::exponent>;
            
            bool checkFactorization() const
            {
                for(auto f = super::begin(); f != super::end(); ++f )
                {
                    if( !carl::is_one( f->first.coefficient() ) )
                        return false;
                }
                return true;
            }
        public:
            
            std::pair<typename super::iterator, bool> insert(typename super::const_iterator _hint, const std::pair<FactorizedPolynomial<P>, carl::exponent>& _val)
            {
                return insert( _hint, std::move(std::pair<FactorizedPolynomial<P>, carl::exponent>( _val.first, _val.second )) );
            }
            
            typename super::iterator insert(typename super::const_iterator _hint, std::pair<FactorizedPolynomial<P>, carl::exponent>&& _val)
            {
                assert( super::find( _val.first ) == super::end() );
                auto result = super::insert( _hint, std::move(_val) );
                assert( checkFactorization() );
                return result;
            }
            
            std::pair<typename super::iterator, bool> insert(const std::pair<FactorizedPolynomial<P>, carl::exponent>& _val)
            {
                return insert( std::move(std::pair<FactorizedPolynomial<P>, carl::exponent>( _val.first, _val.second )) );
            }
            
            std::pair<typename super::iterator, bool> insert(std::pair<FactorizedPolynomial<P>, carl::exponent>&& _val)
            {
                exponent e = _val.second;
                std::pair<typename super::iterator, bool> insertResult = super::insert( std::move(_val) );
                if ( !insertResult.second )
                {
                    //Increment exponent for already existing factor
                    insertResult.first->second += e;
                }
                assert( checkFactorization() );
                return insertResult;
            }
            
            void insert( typename super::const_iterator _first, typename super::const_iterator _last )
            {
                if( super::empty() )
                    super::insert( _first, _last );
                else
                {
                    typename super::iterator lastInserted = super::begin();
                    while( _first != _last )
                    {
                        lastInserted = this->insert( lastInserted, *_first );
                        ++_first;
                    }
                }
                assert( checkFactorization() );
            }
    };
    
    template <typename P>
    std::string factorizationToString( const Factorization<P>& _factorization, bool _infix = true, bool _friendlyVarNames = true );
    
    template <typename P>
    std::ostream& operator<<( std::ostream& _out, const Factorization<P>& _factorization );
    
    template<typename P>
    bool factorizationsEqual( const Factorization<P>& _factorizationA, const Factorization<P>& _factorizationB );
    
    template<typename P>
    class PolynomialFactorizationPair
    {
        friend class FactorizedPolynomial<P>;

    private:
        // Members
        
        /**
         * The hash of this polynomial factorization pair.
         */
        mutable size_t mHash;
        
        /**
         * A mutex for situation where any member is changed.
         */
        mutable std::recursive_mutex mMutex;
        
        /**
         * A factorization (not necessarily the prime factorization) of the polynomial.
         */
        mutable Factorization<P> mFactorization;
        
        /**
         * A pointer to a polynomial. This pointer might be set to nullptr, if the factorization has not yet been expanded.
         */
        mutable P* mpPolynomial;

        /**
         * Indicates, if the polynomial is irreducible
         */
        mutable int mIrreducible;

        template<typename P1>
        friend P1 computePolynomial( const Factorization<P1>& );

        template<typename P1>
        friend P1 computePolynomial( const PolynomialFactorizationPair<P1>& );

        /**
         * Turn (possible) tree structure of factorization into linear list of factors.
         * @return true, if the factorization has been changed;
         *         false, otherwise.
         */
        typename P::CoeffType flattenFactorization() const;
        
        bool checkFactorization() const;
        
        inline bool assertFactorization() const
        {
            return (mpPolynomial == nullptr || computePolynomial( *this ) == *mpPolynomial );
        }

        /**
         * Get the flattened factorization.
         * @return The factorization of this polynomial factorization pair
         */
        const Factorization<P>& factorization() const
        {
            return mFactorization;
        }
        
        inline bool factorizedTrivially() const
        {
            assert( !mFactorization.empty() );
            return this == &mFactorization.begin()->first.content();
        }
        
        void gatherVariables( std::set<carl::Variable>& _vars ) const
        {
            if( mpPolynomial != nullptr )
            {
                mpPolynomial->gatherVariables( _vars );
            }
            else
            {
                for( auto const& factor : mFactorization )
                {
                    factor.first.gatherVariables( _vars );
                }
            }
        }

        /**
         * Set new factorization for polynomial as two factors
         * @param _fpolyA First polynomial.
         * @param exponentA Exponent of first polynomial.
         * @param _fpolyB Second polynomial.
         * @param exponentB Exponent of second polynomial.
         */
        void setNewFactors( const FactorizedPolynomial<P>& _fpolyA, carl::exponent exponentA, const FactorizedPolynomial<P>& _fpolyB, carl::exponent exponentB ) const;

        bool isIrreducible() const;

    public:
        // Constructor.
        PolynomialFactorizationPair() = delete; // no implementation
        /*
         * @param _factorization The factorization. Every factor must be not constant.
         * @param _polynomial Polynomial with Polynomial = Factorization * Coefficient
         */
        explicit PolynomialFactorizationPair( Factorization<P>&& _factorization, P* _polynomial = nullptr );
        PolynomialFactorizationPair( const PolynomialFactorizationPair& ) = delete; // no implementation
        ~PolynomialFactorizationPair();
		
		PolynomialFactorizationPair& operator=(const PolynomialFactorizationPair& pfp) = default;
        
        /**
         * @return The hash of this polynomial factorization pair.
         */
        size_t hash() const
        {
            return mHash;
        }

		const auto& polynomial() const {
			return *mpPolynomial;
		}
        
        /**
         * @param _polyFactA The first polynomial factorization pair to compare.
         * @param _polyFactB The second polynomial factorization pair to compare.
         * @return true, if the two given polynomial factorization pairs are equal.
         */
        template<typename P1>
        friend bool operator==( const PolynomialFactorizationPair<P1>& _polyFactA, const PolynomialFactorizationPair<P1>& _polyFactB );
        
        /**
         * @param _polyFactA The first polynomial factorization pair to compare.
         * @param _polyFactB The second polynomial factorization pair to compare.
         * @return true, if the first given polynomial factorization pair is less than the second given polynomial factorization pair.
         */
        template<typename P1>
        friend bool operator<( const PolynomialFactorizationPair<P1>& _polyFactA, const PolynomialFactorizationPair<P1>& _polyFactB );
    
        /**
         * @param _toUpdate The polynomial factorization pair to be checked for the possibility to be updated.
         * @param _updateWith The polynomial factorization pair used to update the first given one.
         * @return true, if the first polynomial factorization pair can be updated with the second one.
         */
        template<typename P1>
        friend bool canBeUpdated( const PolynomialFactorizationPair<P1>& _toUpdate, const PolynomialFactorizationPair<P1>& _updateWith );
    
        /**
         * Updates the first given polynomial factorization pair with the information stored in the second given polynomial factorization pair.
         * @param _toUpdate The polynomial factorization pair to update with the second given one.
         * @param _updateWith The polynomial factorization pair used to update the first given one.
         */
        template<typename P1>
        friend void update( PolynomialFactorizationPair<P1>& _toUpdate, PolynomialFactorizationPair<P1>& _updateWith );
    
        /**
         * Calculates the factorization of the gcd of the polynomial represented by the two given polynomial factorization pairs. 
         * As a side effect the factorizations of these pairs can be refined. (c.f. Accelerating Parametric Probabilistic Verification, Algorithm 2)
         * @param _pfPairA The first polynomial factorization pair to calculate the gcd with.
         * @param _pfPairB The second polynomial factorization pair to calculate the gcd with.
         * @param _restA The remaining factorization of the first polynomial without the gcd.
         * @param _restB The remaining factorization of the second polynomial without the gcd.
         * @param _coeff 
         * @param _pfPairARefined A bool which is set to true, if the factorization of the first given polynomial factorization pair has been refined.
         * @param _pfPairBRefined A bool which is set to true, if the factorization of the second given polynomial factorization pair has been refined.
         * @return The factorization of the gcd of the polynomial represented by the two given polynomial factorization pairs. 
         */
        template<typename P1>
        friend Factorization<P1> gcd( const PolynomialFactorizationPair<P1>& _pfPairA, const PolynomialFactorizationPair<P1>& _pfPairB, Factorization<P1>& _restA, Factorization<P1>& _restB, typename P1::CoeffType& _coeff, bool& _pfPairARefined, bool& _pfPairBRefined );
        
        /**
         * @param _pfPair The polynomial to calculate the factorization for.
         * @return A factorization of this factorized polynomial. (probably finer than the one factorization() returns)
         */
        template<typename P1>
        friend Factors<FactorizedPolynomial<P1>> factor( const PolynomialFactorizationPair<P1>& _pfPair, const typename P1::CoeffType&  );
        
        /**
         * Prints the given polynomial-factorization pair on the given output stream.
         * @param _out The stream to print on.
         * @param _pfPair The polynomial-factorization pair to print.
         * @return The output stream after inserting the output.
         */
        template <typename P1>
        friend std::ostream& operator<<( std::ostream& _out, const PolynomialFactorizationPair<P1>& _pfPair );
        
        /**
         * Updates the hash.
         */
        void rehash() const;
        
    };
    
    /**
     * Compute the polynomial from the given polynomial-factorization pair.
     * @param _fpPair A polynomial-factorization pair.
     * @return The polynomial.
     */
    template<typename P>
    P computePolynomial( const PolynomialFactorizationPair<P>& _pfPair );
} // namespace carl

namespace std
{
    template<typename P>
    struct hash<carl::PolynomialFactorizationPair<P>>
    {
        size_t operator()( const carl::PolynomialFactorizationPair<P>& _pfp ) const 
        {
            return _pfp.hash();
        }
    };
} // namespace std

#include "PolynomialFactorizationPair.tpp"
