/* 
 * File:   FactorizedPolynomial.h
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 10:55 AM
 */

#pragma once

#include <iostream>
#include <carl/numbers/numbers.h>
#include <carl/util/Cache.h>
#include <carl/core/polynomialfunctions/Division.h>
#include "PolynomialFactorizationPair.h"
#include <carl/core/VariablesInformation.h>

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
        
        /// The ordering of the terms.
        using OrderedBy = typename P::OrderedBy;
        /// Type of the coefficients.
        using CoeffType = typename P::CoeffType;
        /// Type of the terms.
        using TermType = typename P::TermType;
        /// Type of the monomials within the terms.
        using MonomType = typename P::MonomType;
        /// Policies for this monomial.
        using Policy = typename P::Policy;
        /// Number type within the coefficients.
        using NumberType = typename UnderlyingNumberType<CoeffType>::type;
        /// Integer type associated with the number type.
        using IntNumberType = typename IntegralType<NumberType>::type;
        ///
        using PolyType = P;
        ///
        using TermsType = typename P::TermsType;
        ///
        using CACHE = Cache<PolynomialFactorizationPair<P>>;
    
        enum ConstructorOperation : unsigned { ADD, SUB, MUL, DIV };

    private:
        // Members

        /**
         * The reference of the entry in the cache corresponding to this factorized polynomial.
         */
        mutable typename CACHE::Ref mCacheRef;

        /**
         * The cache in which the actual content of this factorized polynomial is stored.
         */
        std::shared_ptr<CACHE> mpCache;

        /**
         * Co-prime coefficient of the factorization
         */
        mutable CoeffType mCoefficient;

        /**
         * Updates the hash of the entry in the cache corresponding to this factorized 
         * polynomial, which is also its hash.
         */
        void rehash() const
        {
            if( mpCache != nullptr )
                mpCache->rehash( mCacheRef );
        }
        
        void strengthenActivity() const
        {
            if( mpCache != nullptr )
                mpCache->strengthenActivity( mCacheRef );
        }

        template<typename P1>
        friend bool existsFactorization( const FactorizedPolynomial<P1>& fpoly )
        {
            assert( fpoly.mpCache == nullptr || fpoly.mCacheRef != Cache<PolynomialFactorizationPair<P1>>::NO_REF );
            return fpoly.mpCache != nullptr;
        }
        
        #define ASSERT_CACHE_EQUAL( _cacheA, _cacheB ) assert( _cacheA == nullptr || _cacheB == nullptr || _cacheA == _cacheB )

        #define ASSERT_CACHE_REF_LEGAL( _fp ) assert( (_fp.pCache() == nullptr) == (_fp.cacheRef() == CACHE::NO_REF) )

        /**
         * Computes the coefficient of the factorization and sets the coefficients of all factors to 1.
         * @param _factorization The factorization.
         * @return The coefficients of the whole factorization.
         */
        template<typename P1>
        friend Coeff<P1> distributeCoefficients( Factorization<P1>& _factorization );

        /**
         * Computes the common divisor with rest of two factorizations.
         * @param _fFactorizationA The factorization of the first polynomial.
         * @param _fFactorizationB The factorization of the second polynomial.
         * @param _fFactorizationRestA Returns the remaining factorization of the first polynomial without the common divisor
         * @param _fFactorizationRestB Returns the remaining factorization of the second polynomial without the common divisor
         * @return The factorization of a common divisor of the two given factorized polynomials.
         */
        template<typename P1>
        friend Factorization<P1> commonDivisor( const FactorizedPolynomial<P1>& _fFactorizationA, const FactorizedPolynomial<P1>& _fFactorizationB, Factorization<P1>& _fFactorizationRestA, Factorization<P1>& _fFactorizationRestB);

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
        friend FactorizedPolynomial<P1> gcd(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB, FactorizedPolynomial<P1>& _fpolyRestA, FactorizedPolynomial<P1>& _fpolyRestB);

    public:

        // Constructors.
        FactorizedPolynomial();
        explicit FactorizedPolynomial( const CoeffType& );
        explicit FactorizedPolynomial( const P& _polynomial, const std::shared_ptr<CACHE>&, bool _polyNormalized = false );
        FactorizedPolynomial( const FactorizedPolynomial<P>& );
        FactorizedPolynomial( FactorizedPolynomial<P>&& );
        explicit FactorizedPolynomial(const std::pair<ConstructorOperation, std::vector<FactorizedPolynomial>>& _p);
        explicit FactorizedPolynomial( Factorization<P>&& _factorization, const CoeffType&, const std::shared_ptr<CACHE>& );
        
        // Destructor.
        ~FactorizedPolynomial();
        
        /**
         * Copies the given factorized polynomial.
         * @param The factorized polynomial to copy.
         * @return A reference to the copy of the given factorized polynomial.
         */
        FactorizedPolynomial<P>& operator=( const FactorizedPolynomial<P>& );
        
        explicit operator PolyType() const
        {
            if( existsFactorization( *this ) )
                return this->polynomial()*this->mCoefficient;
            return PolyType( mCoefficient );
        }
        
        /**
         * @return The reference of the entry in the cache corresponding to this factorized polynomial.
         */
        typename CACHE::Ref cacheRef() const
        {
            return mCacheRef;
        }
        
        /**
         * @return The cache used by this factorized polynomial.
         */
        std::shared_ptr<CACHE> pCache() const
        {
            return mpCache;
        }
        
        /**
         * @return The cache used by this factorized polynomial.
         */
        CACHE& cache() const
        {
            return *mpCache;
        }
        
        /**
         * @return The entry in the cache corresponding to this factorized polynomial.
         */
        const PolynomialFactorizationPair<P>& content() const
        {
            assert( existsFactorization( *this ) );
            return mpCache->get( mCacheRef );
        }
        
        /**
         * @return The hash value of the entry in the cache corresponding to this factorized polynomial.
         */
        size_t getHash() const
        {
            if( existsFactorization( *this ) )
            {
                return mpCache->get( mCacheRef ).getHash();
            }
            return std::hash<CoeffType>()( mCoefficient );
        }
        
        /**
         * Set coefficient
         * @param coeff Coefficient
         */
        //TODO make private and fix friend declaration
        void setCoefficient( CoeffType coeff ) const
        {
            mCoefficient = coeff;
        }

        /**
         * @return The factorization of this polynomial.
         */
        const Factorization<P>& factorization() const
        {
            assert( existsFactorization( *this ) );
            //TODO (matthias) activate?
            CoeffType c = content().flattenFactorization();
            if( c != CoeffType( 0 ) )
            {
                mCoefficient *= c;
                rehash();
            }
            return content().factorization();
        }

        const P& polynomial() const
        {
            assert( existsFactorization( *this ) );
            if( content().mpPolynomial == nullptr )
            {
                content().mpPolynomial = new P( computePolynomial( content().factorization() ) );
                rehash();
            }

            return *content().mpPolynomial;
        }

        /**
         * @return Coefficient of the polynomial.
         */
        const CoeffType& coefficient() const
        {
            return mCoefficient;
        }

        P polynomialWithCoefficient() const
        {
            if(existsFactorization(*this)) {
                return this->mCoefficient * polynomial();
            }
            else {
                return P(mCoefficient);
            }
        }

        /**
         * @return true, if the factorized polynomial is constant.
         */
        bool isConstant() const
        {
            return mpCache == nullptr;
        }

        /**
         * @return true, if the factorized polynomial is one.
         */
        bool isOne() const
        {
            return isConstant() && carl::isOne(mCoefficient);
        }

        /**
         * @return true, if the factorized polynomial is zero.
         */
        bool isZero() const
        {
            return isConstant() && carl::isZero(mCoefficient);
        }
        
        /**
         * Calculates the number of terms. (Note, that this requires to expand the factorization and, thus, can be expensive in the case that 
         * the factorization has not yet been expanded.)
         * @return the number of terms
         */
        size_t nrTerms() const
        {
            return polynomial().nrTerms();
        }
        
        /**
         * @return A rough estimation of the size of this factorized polynomial. If it has already been expanded, the number of terms 
         *         of the expanded form are returned; otherwise the number of terms in the factors.
         */
        size_t size() const
        {
            if( existsFactorization(*this) )
            {
                if( factorizedTrivially() )
                    return polynomial().size();
                size_t result = 0;
                for( const auto& factor : content().factorization() )
                    result += factor.first.size();
                return result;
            }
            return 1;
        }
        
        /**
         * @return An approximation of the complexity of this polynomial.
         */
        size_t complexity() const
        {
            if( existsFactorization(*this) )
            {
                if( factorizedTrivially() )
                    return complexity(polynomial());
                size_t result = 0;
                for( const auto& factor : content().factorization() )
                    result += complexity(factor.first);
                return result;
            }
            return 1;
        }
        
        /**
         * Checks if the polynomial is linear.
         * @return If this is linear.
         */
        bool isLinear() const
        {
            if( existsFactorization(*this) )
            {
                if( factorizedTrivially() )
                    return polynomial().isLinear();
                return false;
            }
            return true;
        }
        
        /**
         * @return The lcm of the denominators of the coefficients in p divided by the gcd of numerators 
         *		 of the coefficients in p.
         */
        template<typename C = CoeffType, EnableIf<is_subset_of_rationals<C>> = dummy>
        CoeffType coprimeFactor() const
        {
            return constant_one<CoeffType>::get()/mCoefficient;
        }
        
        /**
         * @return The lcm of the denominators of the coefficients (without the constant one) in p divided by the gcd of numerators 
         *		 of the coefficients in p.
         */
        template<typename C = CoeffType, EnableIf<is_subset_of_rationals<C>> = dummy>
        CoeffType coprimeFactorWithoutConstant() const;

        /**
         * @return p * p.coprimeFactor()
         * @see coprimeFactor()
         */
        FactorizedPolynomial<P> coprimeCoefficients() const
        {
            if( existsFactorization(*this) )
            {
                FactorizedPolynomial<P> result( *this );
                result.setCoefficient( constant_one<CoeffType>::get() );
                return result;
            }
            return FactorizedPolynomial<P>( constant_one<CoeffType>::get() );
        }
        
        /**
         * @return true, if this factorized polynomial, has only itself as factor.
         */ 
        bool factorizedTrivially() const
        {
            return content().factorizedTrivially();
        }
        
        /**
         * Iterates through all factors and their terms to find variables occurring in this polynomial.
         * @param vars Holds the variables occurring in the polynomial at return.
         */
        void gatherVariables( std::set<carl::Variable>& _vars ) const
        {
            if( mpCache == nullptr )
                return;
            content().gatherVariables( _vars );
        }
        
        std::set<Variable> gatherVariables() const
        {
            std::set<Variable> vars;
            gatherVariables(vars);
            return vars;
        }
        
        /**
         * Retrieves the constant term of this polynomial or zero, if there is no constant term.
         * @reiturn Constant term.
         */
        CoeffType constantPart() const;
        
        /**
         * Calculates the max. degree over all monomials occurring in the polynomial.
         * As the degree of the zero polynomial is \f$-\infty\f$, we assert that this polynomial is not zero. This must be checked by the caller before calling this method.
         * @see @cite GCL92, page 48
         * @return Total degree.
         */
        size_t totalDegree() const;
        
        /**
         * Returns the coefficient of the leading term.
         * Notice that this is not defined for zero polynomials. 
         * @return 
         */
        CoeffType lcoeff() const;
        
        /**
	     * The leading term
	     * @return 
	     */
	    TermType lterm() const;
        
        /**
         * Gives the last term according to Ordering. Notice that if there is a constant part, it is always trailing.
         * @return 
         */
        TermType trailingTerm() const;
        
        /**
         * For terms with exactly one variable, get this variable.
         * @return The only variable occuring in the term.
         */
        Variable getSingleVariable() const
        { 
            assert( existsFactorization( *this ) );
            if( factorizedTrivially() )
                return polynomial().getSingleVariable();
            return content().factorization().begin()->first.getSingleVariable();
        }
        
        /**
         * Checks whether only one variable occurs.
         * @return 
         * Notice that it might be better to use the variable information if several pieces of information are requested.
         */
        bool isUnivariate() const;
        
        UnivariatePolynomial<CoeffType> toUnivariatePolynomial() const
        {
            return std::move(polynomial().toUnivariatePolynomial() *= mCoefficient); // In this case it makes sense to expand the polynomial.
        }
        
        UnivariatePolynomial<FactorizedPolynomial<P>> toUnivariatePolynomial( Variable _var ) const;
        
        /**
         * Checks if the polynomial has a constant term that is not zero.
         * @return If there is a constant term unequal to zero.
         */
        bool hasConstantTerm() const;
        
        /**
         * @param _var The variable to check for its occurrence.
         * @return true, if the variable occurs in this term.
         */
        bool has( Variable _var ) const;
    
        template<bool gatherCoeff>
        VariableInformation<gatherCoeff, FactorizedPolynomial<P>> getVarInfo( Variable _var ) const;

        template<bool gatherCoeff>
        VariablesInformation<gatherCoeff, FactorizedPolynomial<P>> getVarInfo() const
        {
            if( existsFactorization( *this ) )
            {
                VariablesInformation<false, P> vi = polynomial().template getVarInfo<false>();
                std::map<Variable, VariableInformation<false, FactorizedPolynomial<P>>> resultVarInfos;
                for( const auto& varViPair : vi )
                {
                    const auto& varI = varViPair.second;
                    VariableInformation<false, FactorizedPolynomial<P>> viFactorized( varI.maxDegree(), varI.minDegree(), varI.occurence() );
                    resultVarInfos.insert( resultVarInfos.end(), std::make_pair( varViPair.first, std::move( viFactorized ) ) );
                }
                return VariablesInformation<false, FactorizedPolynomial<P>>(std::move(resultVarInfos));
            }
            return VariablesInformation<false, FactorizedPolynomial<P>>();
        }
        
        VariablesInformation<true, FactorizedPolynomial<P>> getVarInfo() const
        {
            if( existsFactorization( *this ) )
            {
                // TODO: Maybe we should use the factorization for collecting degrees and coefficients.
                VariablesInformation<true, P> vi = polynomial().template getVarInfo<true>();
                VariablesInformation<true, FactorizedPolynomial<P>> result;
                for( const auto& varViPair : vi )
                {
                    const auto& varI = varViPair.second;
                    std::map<unsigned,FactorizedPolynomial<P>> coeffs;
                    for( const auto& expCoeffPair : varI.coeffs() )
                    {
                        if( expCoeffPair.second.isConstant() )
                        {
                            coeffs.insert( coeffs.end(), std::make_pair( expCoeffPair.first, FactorizedPolynomial<P>( expCoeffPair.second.constantPart() * mCoefficient ) ) );
                        }
                        else
                        {
                            coeffs.insert( coeffs.end(), std::make_pair( expCoeffPair.first, FactorizedPolynomial<P>( expCoeffPair.second, mpCache ) * mCoefficient ) );
                        }
                    }
                    VariableInformation<true, FactorizedPolynomial<P>> viFactorized( varI.maxDegree(), varI.minDegree(), varI.occurence(), std::move( coeffs ) );
                    result.insert( result.end(), std::make_pair( varViPair.first, std::move( viFactorized ) ) );
                }
                return result;
            }
            return VariablesInformation<false, FactorizedPolynomial<P>>();
        }
        
        /**
         * Retrieves information about the definiteness of the polynomial.
         * @return Definiteness of this.
         */
        Definiteness definiteness( bool _fullEffort = true ) const;
		
		/**
		 * Derivative of the factorized polynomial wrt variable x
		 * @param _var main variable
		 * @param _nth how often should derivative be applied
		 * 
		 * @todo only _nth == 1 is supported
		 * @todo we do not use factorization currently
		 */
		FactorizedPolynomial<P> derivative(const carl::Variable& _var, unsigned _nth = 1) const;
		
		/**
		 * Raise polynomial to the power 
         * @param _exp the exponent of the power
         * @return p^exponent
		 * 
		 * @todo uses multiplication -> bad idea.
         */
		FactorizedPolynomial<P> pow(unsigned _exp) const;

        /**
         * Calculates the square of this factorized polynomial if it is a square.
         * @param _result Used to store the result in.
         * @return true, if this factorized polynomial is a square; false, otherwise.
         */
        bool sqrt( FactorizedPolynomial<P>& _result ) const;
        
        /**
         * Divides the polynomial by the given coefficient.
         * Applies if the coefficients are from a field.
         * @param _divisor
         * @return 
         */
        template<typename C = CoeffType, EnableIf<is_field<C>> = dummy>
        FactorizedPolynomial<P> divideBy( const CoeffType& _divisor ) const;
        
        /**
         * Calculating the quotient and the remainder, such that for a given polynomial p we have
         * p = _divisor * quotient + remainder.
         * @param _divisor Another polynomial
         * @return A divisionresult, holding the quotient and the remainder.
         * @see
         * @note Division is only defined on fields
         */
        DivisionResult<FactorizedPolynomial<P>> divideBy( const FactorizedPolynomial<P>& _divisor ) const;
        
        /**
         * Divides the polynomial by another polynomial.
         * If the divisor divides this polynomial, quotient contains the result of the division and true is returned.
         * Otherwise, false is returned and the content of quotient remains unchanged.
         * Applies if the coefficients are from a field.
         * Note that the quotient must not be *this.
         * @param _divisor
         * @param _quotient
         * @return 
         */
        template<typename C = CoeffType, EnableIf<is_field<C>> = dummy>
        bool divideBy( const FactorizedPolynomial<P>& _divisor, FactorizedPolynomial<P>& _quotient ) const;

        /**
         * Choose a non-null cache from two caches.
         * @param _pCacheA First cache.
         * @param _pCacheB Second cache.
         * @return A non-null cache.
         */
        static std::shared_ptr<CACHE> chooseCache( std::shared_ptr<CACHE> _pCacheA, std::shared_ptr<CACHE> _pCacheB )
        {
            if ( _pCacheA != nullptr )
                return _pCacheA;
            else
            {
                assert( _pCacheB != nullptr );
                return _pCacheB;
            }
        }
        
        /**
         * @param _fpoly The factorized polynomial to retrieve the expanded polynomial for.
         * @return The polynomial (of the underlying polynomial type) when expanding the factorization of the given factorized polynomial.
         */
        template<typename P1>
        friend P1 computePolynomial(const FactorizedPolynomial<P1>& _fpoly );
        
        /**
         * @param _fpoly The operand.
         * @return The given factorized polynomial times -1.
         */
        FactorizedPolynomial<P> operator-() const;
        
        /**
         * @param _coef The summand to add this factorized polynomial with.
         * @return This factorized polynomial after adding the given summand.
         */
        FactorizedPolynomial<P>& operator+=( const CoeffType& _coef );

        /**
         * @param _fpoly The summand to add this factorized polynomial with.
         * @return This factorized polynomial after adding the given summand.
         */
        FactorizedPolynomial<P>& operator+=( const FactorizedPolynomial<P>& _fpoly );

        /**
         * @param _coef The number to subtract from this factorized polynomial.
         * @return This factorized polynomial after subtracting the given number.
         */
        FactorizedPolynomial<P>& operator-=( const CoeffType& _coef );

        /**
         * @param _fpoly The factorized polynomial to subtract from this factorized polynomial.
         * @return This factorized polynomial after adding the given factorized polynomial.
         */
        FactorizedPolynomial<P>& operator-=( const FactorizedPolynomial<P>& _fpoly );

        /**
         * @param _coef The factor to multiply this factorized polynomial with.
         * @return This factorized polynomial after multiplying it with the given factor.
         */
        FactorizedPolynomial<P>& operator*=( const CoeffType& _coef );
        
        /**
         * @param _fpoly The factor to multiply this factorized polynomial with.
         * @return This factorized polynomial after multiplying it with the given factor.
         */
        FactorizedPolynomial<P>& operator*=( const FactorizedPolynomial<P>& _fpoly );

        /** Calculates the quotient. Notice: the divisor has to be a factor of the polynomial.
         * @param _coef The divisor to divide this factorized polynomial with.
         * @return This factorized polynomial after dividing it with the given divisor.
         */
        FactorizedPolynomial<P>& operator/=( const CoeffType& _coef );

        /** Calculates the quotient. Notice: the divisor has to be a factor of the polynomial.
         * @param _fpoly The divisor to divide this factorized polynomial with.
         * @return This factorized polynomial after dividing it with the given divisor.
         */
        FactorizedPolynomial<P>& operator/=( const FactorizedPolynomial<P>& _fpoly );

        /**
         * Calculates the quotient. Notice: the divisor has to be a factor of the polynomial.
         * @param _fdivisor The divisor
         * @return The quotient
         */
        FactorizedPolynomial<P> quotient( const FactorizedPolynomial<P>& _fdivisor ) const;
        
        /**
         * @param _infix
         * @param _friendlyVarNames
         * @return 
         */
        std::string toString( bool _infix = true, bool _friendlyVarNames = true ) const;

        /**
         * Calculates the quotient of the polynomials. Notice: the second polynomial has to be a factor of the first polynomial.
         * @param _fpolyA The dividend.
         * @param _fpolyB The divisor.
         * @return The quotient
         */
        template<typename P1>
        friend FactorizedPolynomial<P1> quotient( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB );

        /**Computes the least common multiple of two given polynomials. The method refines the factorization.
         * @param _fpolyA The first factorized polynomial to compute the lcm for.
         * @param _fpolyB The second factorized polynomial to compute the lcm for.
         * @return The lcm of the two given factorized polynomials.
         */
        template<typename P1>
        friend FactorizedPolynomial<P1> lcm( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB );

        /**
         * @param _fpolyA The first factorized polynomial to compute the common divisor for.
         * @param _fpolyB The second factorized polynomial to compute the common divisor for.
         * @return A common divisor of the two given factorized polynomials.
         */
        template<typename P1>
        friend FactorizedPolynomial<P1> commonDivisor( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB );

        /**
         * @param _fpolyA The first factorized polynomial to compute the common multiple for.
         * @param _fpolyB The second factorized polynomial to compute the common multiple for.
         * @return A common multiple of the two given factorized polynomials.
         */
        template<typename P1>
        friend FactorizedPolynomial<P1> commonMultiple( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB );

        /**
         * Determines the greatest common divisor of the two given factorized polynomials. The method exploits the partial factorization
         * stored in the arguments and refines it. (c.f. Accelerating Parametric Probabilistic Verification, Section 4)
         * @param _fpolyA The first factorized polynomial to compute the greatest common divisor for.
         * @param _fpolyB The second factorized polynomial to compute the greatest common divisor for.
         * @return The greatest common divisor of the two given factorized polynomials.
         */
        template<typename P1>
        friend FactorizedPolynomial<P1> gcd(const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB);
        
        /**
         * Divides each of the two given factorized polynomials by their common factors of their (partial) factorization.
         * @param _fpolyA The first factorized polynomial.
         * @param _fpolyB The second factorized polynomial.
         * @return The pair of the resulting factorized polynomials.
         */
        template<typename P1>
        friend std::pair<FactorizedPolynomial<P1>,FactorizedPolynomial<P1>> lazyDiv( const FactorizedPolynomial<P1>& _fpolyA, const FactorizedPolynomial<P1>& _fpolyB );
        
        /**
         * @param _fpoly The polynomial to calculate the factorization for.
         * @return A factorization of this factorized polynomial. (probably finer than the one factorization() returns)
         */
        template<typename P1>
        friend Factors<FactorizedPolynomial<P1>> factor( const FactorizedPolynomial<P1>& _fpoly );
        
        // Operators which need to be friend.
        template <typename P1>
        friend FactorizedPolynomial<P1> operator+(const FactorizedPolynomial<P1>& _lhs, const FactorizedPolynomial<P1>& _rhs);

        template <typename P1>
        friend FactorizedPolynomial<P1> operator+(const FactorizedPolynomial<P1>& _lhs, const typename FactorizedPolynomial<P1>::CoeffType& _rhs);
        
        template <typename P1>
        friend FactorizedPolynomial<P1> operator-(const FactorizedPolynomial<P1>& _lhs, const FactorizedPolynomial<P1>& _rhs);

        template <typename P1>
        friend FactorizedPolynomial<P1> operator-(const FactorizedPolynomial<P1>& _lhs, const typename FactorizedPolynomial<P1>::CoeffType& _rhs);
        
        template <typename P1>
        friend FactorizedPolynomial<P1> operator*(const FactorizedPolynomial<P1>& _lhs, const FactorizedPolynomial<P1>& _rhs);

        template <typename P1>
        friend FactorizedPolynomial<P1> operator*(const FactorizedPolynomial<P1>& _lhs, const typename FactorizedPolynomial<P1>::CoeffType& _rhs);
    };

/**
 * @return true, if the factorized polynomial is one.
 */
template <typename P>
bool isOne(const FactorizedPolynomial<P>& fp) {
	return fp.isOne();
}

/**
 * @return true, if the factorized polynomial is zero.
 */
template <typename P>
bool isZero(const FactorizedPolynomial<P>& fp) {
	return fp.isZero();
}
    
    /**
     * Obtains the polynomial (representation) of this factorized polynomial. Note, that the result won't be stored
     * in the factorized polynomial, hence, this method should only be called for debug purpose.
     * @param _fpoly The factorized polynomial to get its polynomial (representation) for.
     * @return The polynomial (representation) of this factorized polynomial
     */
    template<typename P>
    P computePolynomial(const FactorizedPolynomial<P>& _fpoly );

    /**
     * Prints the factorization representation of the given factorized polynomial on the given output stream.
     * @param _out The stream to print on.
     * @param _fpoly The factorized polynomial to print.
     * @return The output stream after inserting the output.
     */
    template <typename P>
    std::ostream& operator<<( std::ostream& _out, const FactorizedPolynomial<P>& _fpoly );

    template<typename P> struct needs_cache<FactorizedPolynomial<P>>: std::true_type {};
    
    template<typename P> struct is_factorized<FactorizedPolynomial<P>>: std::true_type {};
    
    /// @name Equality comparison operators
	/// @{
	/**
	 * Checks if the two arguments are equal.
	 * @param _lhs First argument.
	 * @param _rhs Second argument.
	 * @return `_lhs == _rhs`
	 */
	template <typename P>
	bool operator==(const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs);
    
	template <typename P>
	bool operator==(const FactorizedPolynomial<P>& _lhs, const typename FactorizedPolynomial<P>::CoeffType& _rhs);
    
	template <typename P>
	inline bool operator==(const typename FactorizedPolynomial<P>::CoeffType& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
		return _rhs == _lhs;
	}
	/// @}

	/// @name Inequality comparison operators
	/// @{
	/**
	 * Checks if the two arguments are not equal.
	 * @param _lhs First argument.
	 * @param _rhs Second argument.
	 * @return `_lhs != _rhs`
	 */
	template <typename P>
	inline bool operator!=(const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs);
    
	template <typename P>
	inline bool operator!=(const FactorizedPolynomial<P>& _lhs, const typename FactorizedPolynomial<P>::CoeffType& _rhs)
    {
		return !(_lhs == _rhs);
	}
    
	template <typename P>
	inline bool operator!=(const typename FactorizedPolynomial<P>::CoeffType& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
		return !(_lhs == _rhs);
	}
	/// @}

	/// @name Less than comparison operators
	/// @{
	/**
	 * Checks if the first arguments is less than the second.
	 * @param _lhs First argument.
	 * @param _rhs Second argument.
	 * @return `_lhs < _rhs`
	 */
	template <typename P>
	bool operator<(const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs);
    
	template <typename P>
	bool operator<(const FactorizedPolynomial<P>& _lhs, const typename FactorizedPolynomial<P>::CoeffType& _rhs);
    
	template <typename P>
	inline bool operator<(const typename FactorizedPolynomial<P>::CoeffType& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
        return _rhs < _lhs;
    }
	/// @}

	/// @name Less or equal comparison operators
	/// @{
	/**
	 * Checks if the first arguments is less or equal than the second.
	 * @param _lhs First argument.
	 * @param _rhs Second argument.
	 * @return `_lhs <= _rhs`
	 */
	template <typename P>
	inline bool operator<=(const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
		return !(_rhs < _lhs);
	}
    
	template <typename P>
	inline bool operator<=(const FactorizedPolynomial<P>& _lhs, const typename FactorizedPolynomial<P>::CoeffType& _rhs)
    {
		return !(_rhs < _lhs);
	}
    
	template <typename P>
	inline bool operator<=(const typename FactorizedPolynomial<P>::CoeffType& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
		return !(_rhs < _lhs);
	}
	/// @}

	/// @name Greater than comparison operators
	/// @{
	/**
	 * Checks if the first arguments is greater than the second.
	 * @param _lhs First argument.
	 * @param _rhs Second argument.
	 * @return `_lhs > _rhs`
	 */
	template <typename P>
	inline bool operator>(const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
		return _rhs < _lhs;
	}
    
	template <typename P>
	inline bool operator>(const FactorizedPolynomial<P>& _lhs, const typename FactorizedPolynomial<P>::CoeffType& _rhs)
    {
		return _rhs < _lhs;
	}
    
	template <typename P>
	inline bool operator>(const typename FactorizedPolynomial<P>::CoeffType& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
		return _rhs < _lhs;
	}
	/// @}

	/// @name Greater or equal comparison operators
	/// @{
	/**
	 * Checks if the first arguments is greater or equal than the second.
	 * @param _lhs First argument.
	 * @param _rhs Second argument.
	 * @return `_lhs >= _rhs`
	 */
	template <typename P>
	inline bool operator>=(const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
		return _rhs <= _lhs;
	}
    
	template <typename P>
	inline bool operator>=(const FactorizedPolynomial<P>& _lhs, const typename FactorizedPolynomial<P>::CoeffType& _rhs)
    {
		return _rhs <= _lhs;
	}
    
	template <typename P>
	inline bool operator>=(const typename FactorizedPolynomial<P>::CoeffType& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
		return _rhs <= _lhs;
	}
	/// @}
	
	/// @name Addition operators
	/// @{
	/**
	 * Performs an addition involving a polynomial.
	 * @param _lhs First argument.
	 * @param _rhs Second argument.
	 * @return `_lhs + _rhs`
	 */
	template <typename P>
	FactorizedPolynomial<P> operator+(const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs);
    
	template <typename P>
	FactorizedPolynomial<P> operator+(const FactorizedPolynomial<P>& _lhs, const typename FactorizedPolynomial<P>::CoeffType& _rhs);
    
	template <typename P>
	inline FactorizedPolynomial<P> operator+(const typename FactorizedPolynomial<P>::CoeffType& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
		return std::move(_rhs + _lhs);
	}
	/// @}
	
	/// @name Subtraction operators
	/// @{
	/**
	 * Performs an subtraction involving a polynomial.
	 * @param _lhs First argument.
	 * @param _rhs Second argument.
	 * @return `_lhs - _rhs`
	 */
	template <typename P>
	FactorizedPolynomial<P> operator-(const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs);
    
	template <typename P>
	FactorizedPolynomial<P> operator-(const FactorizedPolynomial<P>& _lhs, const typename FactorizedPolynomial<P>::CoeffType& _rhs);
    
	template <typename P>
	inline FactorizedPolynomial<P> operator-(const typename FactorizedPolynomial<P>::CoeffType& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
		return std::move( -_rhs + _lhs );
	}
	/// @}
	
	/// @name Multiplication operators
	/// @{
	/**
	 * Perform a multiplication involving a polynomial.
	 * @param _lhs Left hand side.
	 * @param _rhs Right hand side.
	 * @return `_lhs * _rhs`
	 */
	template <typename P>
	FactorizedPolynomial<P> operator*(const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs);
    
	template <typename P>
	FactorizedPolynomial<P> operator*(const FactorizedPolynomial<P>& _lhs, const typename FactorizedPolynomial<P>::CoeffType& _rhs);
    
	template <typename P>
	inline FactorizedPolynomial<P> operator*(const typename FactorizedPolynomial<P>::CoeffType& _lhs, const FactorizedPolynomial<P>& _rhs)
    {
		return std::move(_rhs * _lhs);
	}
    
	template <typename P>
	inline FactorizedPolynomial<P> operator/(const FactorizedPolynomial<P>& _lhs, const typename FactorizedPolynomial<P>::CoeffType& _rhs )
    {
		return FactorizedPolynomial<P>(_lhs)/=_rhs;
	}
	/// @}
    
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
