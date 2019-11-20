/* 
 * File:   FactorizedPolynomial.tpp
 * Author: Florian Corzilius
 *
 * Created on September 4, 2014, 10:55 AM
 */

#include "FactorizedPolynomial.h"

#include <carl/core/polynomialfunctions/Division.h>
#include <carl/core/polynomialfunctions/Substitution.h>
#include <carl/core/UnivariatePolynomial.h>

#pragma once

namespace carl
{
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial():
        mCacheRef( CACHE::NO_REF ),
        mpCache( nullptr ),
        mCoefficient( 0 )
    {
        assert( mpCache == nullptr || mCacheRef != CACHE::NO_REF );
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const CoeffType& _coefficient ):
        mCacheRef( CACHE::NO_REF ),
        mpCache( nullptr ),
        mCoefficient( _coefficient )
    {
    }

    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const P& _polynomial, const std::shared_ptr<CACHE>& _pCache, bool _polyNormalized ):
        mCacheRef( CACHE::NO_REF ),
        mpCache( carl::isZero(_polynomial) ? nullptr : _pCache ),
        mCoefficient( carl::isZero(_polynomial) ? CoeffType(0) : (_polyNormalized ? CoeffType(1) : CoeffType(1)/_polynomial.coprimeFactor()) )
    {
        assert( !_polyNormalized || (_polynomial.coprimeFactor() == CoeffType(1)) );
        if ( _polynomial.isConstant() )
        {
            mpCache = nullptr;
        }
        else
        {
            P poly = _polyNormalized ? _polynomial : P(_polynomial * (CoeffType(1) / mCoefficient));
            if ( !_polyNormalized && poly.lcoeff() < 0 )
            {
                poly *= CoeffType(-1);
                mCoefficient *= CoeffType(-1);
            }
            /*
             * The following is not very nice, but we know, that the hash won't change, once the polynomial
             * representation is fixed, so we can add the factorizations content belatedly. It is necessary to do so
             * as otherwise the factorized polynomial (this) being the only factor, is not yet cached which leads to an assertion.
             */
            if ( _polyNormalized || carl::isOne(mCoefficient) )
            {
                assert( mpCache != nullptr );
                Factorization<P> factorization;
                PolynomialFactorizationPair<P>* pfPair = new PolynomialFactorizationPair<P>( std::move( factorization), new P(poly) );
                //Factorization is not set yet
                auto ret = mpCache->cache( pfPair );//, &carl::canBeUpdated, &carl::update );
                mCacheRef = ret.first;
                mpCache->reg( mCacheRef );
                if( ret.second )
                {
                    assert( content().mFactorization.empty() );
                    CARL_LOG_DEBUG("carl.core.factorizedpolynomial", "Adding single factor ( " << poly << " )^1");
                    content().mFactorization.insert( std::make_pair( *this, 1 ) );
                }
                else
                {
                    delete pfPair;
                }
            }
            else
            {
                // Factor is polynomial without coefficient
                FactorizedPolynomial factor( poly, mpCache, true );
                mCacheRef = factor.mCacheRef;
                mpCache->reg( mCacheRef );
            }
            //We can not check the factorization yet, but as we have set it, it should be correct.
            //pfPair->assertFactorization();
        }
        ASSERT_CACHE_REF_LEGAL( (*this) );
        assert(computePolynomial(*this) == _polynomial);
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( Factorization<P>&& _factorization, const CoeffType& _coefficient, const std::shared_ptr<CACHE>& _pCache ):
        mCacheRef( CACHE::NO_REF ),
        mpCache( _pCache ),
        mCoefficient( _coefficient )
    {
        assert( !carl::isZero(_coefficient) );
        if ( _factorization.empty() )
            mpCache = nullptr;
        else
        {
            assert( mpCache != nullptr );
            // TODO expensive
            for ( auto factor = _factorization.begin(); factor != _factorization.end(); factor++ )
                assert( carl::isOne(factor->first.coefficient()) );
            PolynomialFactorizationPair<P>* pfPair = new PolynomialFactorizationPair<P>( std::move( _factorization ) );
            auto ret = mpCache->cache( pfPair );//, &carl::canBeUpdated, &carl::update );
            mCacheRef = ret.first;
            mpCache->reg( mCacheRef );
            if( !ret.second )
            {
                delete pfPair;
            }
        }
        ASSERT_CACHE_REF_LEGAL( (*this) );
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( const FactorizedPolynomial<P>& _toCopy ):
        mCacheRef( _toCopy.mCacheRef ),
        mpCache( _toCopy.mpCache ),
        mCoefficient( _toCopy.mCoefficient )
    {
        if ( mpCache != nullptr )
        {
            mpCache->reg( mCacheRef );
        }
        ASSERT_CACHE_REF_LEGAL( (*this) );
    }
	
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial( FactorizedPolynomial<P>&& rhs ):
        mCacheRef( rhs.mCacheRef ),
        mpCache( rhs.mpCache ),
        mCoefficient( rhs.mCoefficient )
    {
        ASSERT_CACHE_REF_LEGAL( (*this) );
        rhs.mCacheRef = CACHE::NO_REF;
        rhs.mpCache = nullptr;
        rhs.mCoefficient = 0;
    }
    
    template<typename P>
    FactorizedPolynomial<P>::FactorizedPolynomial(const std::pair<ConstructorOperation, std::vector<FactorizedPolynomial>>& _pair):
        FactorizedPolynomial<P>::FactorizedPolynomial()
    {
        auto op = _pair.first;
        auto sub = _pair.second;
        assert(!sub.empty());
        auto it = sub.begin();
        
        *this = *it;
        if ((op == ConstructorOperation::SUB) && (sub.size() == 1)) {
            // special treatment of unary minus
            *this = -(*this);
            return;
        }
        if( op == ConstructorOperation::DIV )
        {
            // division shall have at least two arguments
            assert(sub.size() >= 2);
        }
        for( ++it; it != sub.end(); ++it)
        {
            switch( op )
            {
                case ConstructorOperation::ADD:
                    *this += *it;
                    break;
                case ConstructorOperation::SUB:
                    *this -= *it;
                    break;
                case ConstructorOperation::MUL:
                    *this *= *it;
                    break;
                case ConstructorOperation::DIV:
                    assert(it->isConstant());
                    *this /= it->constantPart();
                    break;
              }
        }
    }
    
    template<typename P>
    FactorizedPolynomial<P>::~FactorizedPolynomial()
    {
        if ( mpCache != nullptr )
        {
            mpCache->dereg( mCacheRef );
            mpCache = nullptr;
        }
    }
    
    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator=( const FactorizedPolynomial<P>& _fpoly )
    {
        CARL_LOG_DEBUG("carl.core.factorizedpolynomial", "Copying " << _fpoly);
        ASSERT_CACHE_EQUAL( mpCache, _fpoly.pCache() );
        mCoefficient = _fpoly.mCoefficient;
        if( mCacheRef != _fpoly.cacheRef() )
        {
            if( mpCache != nullptr )
            {
                mpCache->dereg( mCacheRef );
                mCacheRef = _fpoly.cacheRef();
                assert( _fpoly.pCache() == nullptr || mCacheRef != CACHE::NO_REF );
                if( _fpoly.pCache() != nullptr )
                    mpCache->reg( mCacheRef );
                else
                    mpCache = nullptr;
            }
            else if( _fpoly.mpCache != nullptr )
            {
                mpCache = _fpoly.mpCache;
                mCacheRef = _fpoly.cacheRef();
                mpCache->reg( mCacheRef );
            }
        }
        else if( mpCache != nullptr )
        {
            mpCache->dereg(mCacheRef);
            mCacheRef = _fpoly.cacheRef();
            mpCache->reg( mCacheRef );
        }
        ASSERT_CACHE_REF_LEGAL( (*this) );
        assert(computePolynomial(*this) == computePolynomial(_fpoly));
        CARL_LOG_DEBUG("carl.core.factorizedpolynomial", "Done.");
        return *this;
    }
    
    template<typename P>
    template<typename C, EnableIf<is_subset_of_rationals<C>>>
    typename FactorizedPolynomial<P>::CoeffType FactorizedPolynomial<P>::coprimeFactorWithoutConstant() const
    {
        if( existsFactorization( *this ) )
        {
            if( factorizedTrivially() )
            {
                assert( computePolynomial(*this).coprimeFactorWithoutConstant() == polynomial().coprimeFactorWithoutConstant() / mCoefficient );
                return polynomial().coprimeFactorWithoutConstant() / mCoefficient;
            }
            auto factor = content().factorization().begin();
            CoeffType cf = factor->first.coprimeFactorWithoutConstant();
            auto resultNum = carl::getNum( cf );
            auto resultDenom = carl::getDenom( cf );
            ++factor;
            while( factor != content().factorization().end() )
            {
                cf = factor->first.coprimeFactorWithoutConstant();
                resultNum = carl::lcm( resultNum, carl::getNum( cf ) );
                resultDenom = carl::gcd( resultDenom, carl::getDenom( cf ) );
                ++factor;
            }
            assert( computePolynomial(*this).coprimeFactorWithoutConstant() == (resultNum / (mCoefficient*resultDenom)) );
            return resultNum / (mCoefficient*resultDenom);
        }
        return constant_zero<CoeffType>::get();
    }
    
    template<typename P>
    typename FactorizedPolynomial<P>::CoeffType FactorizedPolynomial<P>::constantPart() const
    {
        if( existsFactorization( *this ) )
        {
            if( factorizedTrivially() )
                return polynomial().constantPart() * mCoefficient;
            CoeffType result( mCoefficient );
            for( const auto& factor : content().factorization() )
            {
                result *= carl::pow( factor.first.constantPart(), factor.second );
            }
            assert( computePolynomial(*this).constantPart() == result );
            return result;
        }
        return mCoefficient;
    }

    template<typename P>
    typename FactorizedPolynomial<P>::CoeffType FactorizedPolynomial<P>::lcoeff() const
    {
        if( existsFactorization( *this ) )
        {
            if( factorizedTrivially() )
                return polynomial().lcoeff() * mCoefficient;
            CoeffType result( mCoefficient );
            for( const auto& factor : content().factorization() )
            {
                result *= carl::pow( factor.first.lcoeff(), factor.second );
            }
            assert( computePolynomial(*this).lcoeff() == result );
            return result;
        }
        return mCoefficient;
    }

    template<typename P>
    size_t FactorizedPolynomial<P>::totalDegree() const
    {
        if( existsFactorization( *this ) )
        {
            if( factorizedTrivially() )
                return polynomial().totalDegree();
            exponent result = 0;
            for( const auto& factor : content().factorization() )
            {
                result += factor.first.totalDegree() * factor.second;
            }
            assert( computePolynomial(*this).totalDegree() == result );
            return result;
        }
        assert( !carl::isZero(mCoefficient) );
        return 0;
    }

    template<typename P>
    typename FactorizedPolynomial<P>::TermType FactorizedPolynomial<P>::lterm() const
    {   
        if( existsFactorization( *this ) )
        {
            if( factorizedTrivially() )
                return polynomial().lterm() * mCoefficient;
            TermType result( mCoefficient );
            for( const auto& factor : content().factorization() )
            {
                result *= factor.first.lterm().pow( factor.second );
            }
            assert( computePolynomial(*this).lterm() == result );
            return result;
        }
        return TermType( mCoefficient );
    }

    template<typename P>
    typename FactorizedPolynomial<P>::TermType FactorizedPolynomial<P>::trailingTerm() const
    {
        if( existsFactorization( *this ) )
        {
            if( factorizedTrivially() )
            {
                return polynomial().trailingTerm() * mCoefficient;
            }
            TermType result( mCoefficient );
            for( const auto& factor : content().factorization() )
            {
                result *= factor.first.trailingTerm().pow( factor.second );
            }
            assert( computePolynomial(*this).trailingTerm() == result );
            return result;
        }
        return TermType( mCoefficient );
    }
    
    template<typename P>
    bool FactorizedPolynomial<P>::isUnivariate() const
    {
        if( existsFactorization( *this ) )
        {
            if( factorizedTrivially() )
            {
                return polynomial().isUnivariate();
            }
            Variable foundVar = Variable::NO_VARIABLE;
            for( const auto& factor : content().factorization() )
            {
                if( factor.first.isUnivariate() )
                {
                    if( foundVar == Variable::NO_VARIABLE )
                    {
                        foundVar = factor.first.getSingleVariable();
                    }
                    else if( foundVar != factor.first.getSingleVariable() )
                    {
                        return false;
                    }
                }
                else
                    return false;
            }
            return true;
        }
        return true;
    }
    
    template<typename P>
    UnivariatePolynomial<FactorizedPolynomial<P>> FactorizedPolynomial<P>::toUnivariatePolynomial( Variable _var ) const
    {
        // TODO: This should maybe done directly on the factorization.
        UnivariatePolynomial<P> univPol = polynomial().toUnivariatePolynomial( _var );
        std::vector<FactorizedPolynomial<P>> resultCoeffs;
        for( const P& coeff : univPol.coefficients() )
        {
            resultCoeffs.push_back( std::move(FactorizedPolynomial<P>( coeff, mpCache ) *= mCoefficient) );
        }
        return UnivariatePolynomial<FactorizedPolynomial<P>>( _var, std::move( resultCoeffs ) );
    }
    
    template<typename P>
    bool FactorizedPolynomial<P>::hasConstantTerm() const
    {
        if( existsFactorization( *this ) )
        {
            assert( !carl::isZero( mCoefficient ) );
            if( factorizedTrivially() )
                return polynomial().hasConstantTerm();
            TermType result( mCoefficient );
            for( const auto& factor : content().factorization() )
            {
                if( !factor.first.hasConstantTerm() )
                {
                    assert( !computePolynomial(*this).hasConstantTerm() );
                    return false;
                }
            }
            assert( computePolynomial(*this).hasConstantTerm() );
            return true;
        }
        return !carl::isZero( mCoefficient );
    }

    template<typename P>
    bool FactorizedPolynomial<P>::has( Variable _var ) const
    {   
        if( existsFactorization( *this ) )
        {
            if( factorizedTrivially() )
                return polynomial().has( _var );
            for( const auto& factor : content().factorization() )
            {
                if( factor.first.has( _var ) )
                    return true;
            }
            return false;
        }
        return false;
    }
    
    template<typename P>
    template<bool gatherCoeff>
    VariableInformation<gatherCoeff, FactorizedPolynomial<P>> FactorizedPolynomial<P>::getVarInfo(Variable var) const
    {
        // TODO: Maybe we should use the factorization for collecting degrees and coefficients.
        VariableInformation<gatherCoeff, P> vi = polynomial().template getVarInfo<gatherCoeff>( var );
        if( gatherCoeff )
        {
            std::map<unsigned,FactorizedPolynomial<P>> coeffs;
            for( auto const& expCoeffPair : vi.coeffs() )
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
            return VariableInformation<gatherCoeff, FactorizedPolynomial<P>>( vi.maxDegree(), vi.minDegree(), vi.occurence(), std::move( coeffs ) );
        }
        return VariableInformation<false, FactorizedPolynomial<P>>( vi.maxDegree(), vi.minDegree(), vi.occurence() );

    }
    
    template<typename P>
    Definiteness FactorizedPolynomial<P>::definiteness( bool _fullEffort ) const
    {
        if( existsFactorization( *this ) )
        {
            if( factorizedTrivially() )
                return polynomial().definiteness();
            Definiteness result = Definiteness::POSITIVE;
            for( const auto& factor : content().factorization() )
            {
                Definiteness factorDefiniteness = factor.first.definiteness();
                if( factorDefiniteness == Definiteness::NON )
                    return Definiteness::NON;
                if( factor.second % 2 == 0 )
                {
                    if( factorDefiniteness == Definiteness::NEGATIVE_SEMI )
                        factorDefiniteness = Definiteness::POSITIVE_SEMI;
                    if( factorDefiniteness == Definiteness::NEGATIVE )
                        factorDefiniteness = Definiteness::POSITIVE;
                }
                switch( result )
                {
                    case Definiteness::POSITIVE:
                    {
                        result = factorDefiniteness;
                        break;
                    }
                    case Definiteness::POSITIVE_SEMI:
                    {
                        if( result == Definiteness::NEGATIVE_SEMI && factorDefiniteness == Definiteness::NEGATIVE )
                        {
                            result = Definiteness::NEGATIVE_SEMI;
                        }
                        break;
                    }
                    case Definiteness::NEGATIVE:
                    {
                        switch( factorDefiniteness )
                        {
                            case Definiteness::NEGATIVE_SEMI:
                            {
                                result = Definiteness::POSITIVE_SEMI;
                                break;
                            }
                            case Definiteness::NEGATIVE:
                            {
                                result = Definiteness::POSITIVE;
                                break;
                            }
                            case Definiteness::POSITIVE:
                            {
                                result = Definiteness::NEGATIVE;
                                break;
                            }
                            default:
                            {
                                assert( factorDefiniteness == Definiteness::POSITIVE_SEMI );
                                result = Definiteness::NEGATIVE_SEMI;
                                break;
                            }
                        }
                        break;
                    }
                    default:
                    {
                        assert( result == Definiteness::NEGATIVE_SEMI );
                        if( result == Definiteness::NEGATIVE_SEMI && factorDefiniteness == Definiteness::NEGATIVE )
                        {
                            result = Definiteness::POSITIVE_SEMI;
                        }
                        break;
                    }
                }
            }
            assert( result == Definiteness::NON || computePolynomial(*this).definiteness() == Definiteness::NON || result == computePolynomial(*this).definiteness() );
            return result;
        }
        return mCoefficient < 0 ? Definiteness::NEGATIVE : (mCoefficient > 0 ? Definiteness::POSITIVE : Definiteness::POSITIVE_SEMI);
    }    
    
	template<typename P>
    FactorizedPolynomial<P> FactorizedPolynomial<P>::derivative( const carl::Variable& _var, unsigned _nth ) const
    {
		assert(_nth == 1);
		if (this->isConstant()) {
			return FactorizedPolynomial<P>( constant_zero<CoeffType>::get() );
		}
		// TODO VERY NAIVE
        FactorizedPolynomial<P> result(carl::derivative(polynomial(), _var), mpCache);
        result *= coefficient();
		return result;
	}
	
	template<typename P>
	FactorizedPolynomial<P> FactorizedPolynomial<P>::pow(unsigned _exp) const
    {
		if( _exp == 0 )
            return FactorizedPolynomial<P>( constant_one<CoeffType>::get() );
        if( isZero() )
            return FactorizedPolynomial<P>( constant_zero<CoeffType>::get() );
        FactorizedPolynomial<P> res( constant_one<CoeffType>::get() );
        FactorizedPolynomial<P> mult( *this );
        while( _exp > 0 )
        {
            if( (_exp & 1) != 0 )
                res *= mult;
            _exp /= 2;
            if( _exp > 0 )
                mult *= mult;
        }
        return res;
	}

    template<typename P>
    bool FactorizedPolynomial<P>::sqrt( FactorizedPolynomial<P>& _result ) const
    {
        CoeffType resultCoeff;
        if( !carl::sqrt_exact( mCoefficient, resultCoeff ) )
            return false;
        if( !existsFactorization( *this ) )
        {
            _result = FactorizedPolynomial<P>( resultCoeff );
            return true;
        }
        if( factorizedTrivially() )
        {
            PolyType sqrtOfPolynomial;
            if( polynomial().sqrt( sqrtOfPolynomial ) )
            {
                _result = FactorizedPolynomial<P>( std::move( sqrtOfPolynomial ), mpCache );
                content().setNewFactors( _result, 1, _result, 1 );
                rehash();
                _result *= resultCoeff;
                assert( computePolynomial( _result ).pow( 2 ) == computePolynomial( *this ) );
                return true;
            }
            return false;
        }
        Factorization<P> resultFactorization;
        for( const auto& factor : content().factorization() )
        {
            if( factor.second % 2 == 0 )
            {
                resultFactorization.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( factor.first, factor.second/2 ) );
            }
            else
            {
                FactorizedPolynomial<P> sqrtOfFactor;
                if( !factor.first.sqrt( sqrtOfFactor ) )
                    return false;
                resultFactorization.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( sqrtOfFactor, factor.second ) );
            }
        }
        _result = FactorizedPolynomial<P>( std::move( resultFactorization ), resultCoeff, mpCache );
        assert( computePolynomial( _result ).pow( 2 ) == computePolynomial( *this ) );
        return true;
    }
    
    template<typename P>
    DivisionResult<FactorizedPolynomial<P>> FactorizedPolynomial<P>::divideBy( const FactorizedPolynomial<P>& _divisor ) const
    {
        static_assert(is_field<CoeffType>::value, "Division only defined for field coefficients");
        if( _divisor.isOne() || this->isZero() )
            return DivisionResult<FactorizedPolynomial<P>>( *this, FactorizedPolynomial<P>() );
        if( this->isConstant() && _divisor.isConstant() )
            return DivisionResult<FactorizedPolynomial<P>>( FactorizedPolynomial<P>( this->coefficient()/_divisor.coefficient() ), FactorizedPolynomial<P>() );
        else if( _divisor.isConstant() )
            return DivisionResult<FactorizedPolynomial<P>>( (*this)/_divisor.coefficient(), FactorizedPolynomial<P>() );
        else if( this->isConstant() )
            return DivisionResult<FactorizedPolynomial<P>>( FactorizedPolynomial<P>(), *this );
        std::pair<FactorizedPolynomial<P>,FactorizedPolynomial<P>> ret = lazyDiv( *this, _divisor );
        // TODO: maybe apply gcd, which might make this operation more expensive but a long-term investment
        DivisionResult<P> dr = ret.first.polynomial().divideBy( ret.second.polynomial() );
        // TODO: Could we calculate the quotient and remainder directly on the factorizations?
        FactorizedPolynomial<P> q = dr.quotient.isConstant() ? FactorizedPolynomial<P>( dr.quotient.constantPart() ) : FactorizedPolynomial<P>( dr.quotient, mpCache );
        FactorizedPolynomial<P> r = dr.remainder.isConstant() ? FactorizedPolynomial<P>( dr.remainder.constantPart() ) : FactorizedPolynomial<P>( dr.remainder, mpCache );
        return DivisionResult<FactorizedPolynomial<P>>( q, r );
    }
    
    template<typename P>
    template<typename C, EnableIf<is_field<C>>>
    FactorizedPolynomial<P> FactorizedPolynomial<P>::divideBy( const CoeffType& _divisor ) const
    {
        FactorizedPolynomial<P> result( *this );
        result /= _divisor;
        return std::move( result );
    }
    
    template<typename P>
    template<typename C, EnableIf<is_field<C>>>
    bool FactorizedPolynomial<P>::divideBy( const FactorizedPolynomial<P>& _divisor, FactorizedPolynomial<P>& _quotient ) const
    {
        static_assert(is_field<C>::value, "Division only defined for field coefficients");
        DivisionResult<FactorizedPolynomial<P>> dr = this->divideBy( _divisor );
        if( dr.remainder.isZero() )
        {
            _quotient = dr.quotient;
            return true;
        }
        return false;
    }
	
    template<typename P>
    bool operator==( const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _fpolyB )
    {
        if( _lhs.pCache() == nullptr && _fpolyB.pCache() == nullptr )
        {
            return _lhs.coefficient() == _fpolyB.coefficient();
        }
        else if( _lhs.pCache() != nullptr && _fpolyB.pCache() != nullptr )
        {
            if ( _lhs.coefficient() == _fpolyB.coefficient() )
                return _lhs.content() == _fpolyB.content();
        }
        return false;
    }
    
	template <typename P>
	bool operator==( const FactorizedPolynomial<P>& _lhs, const typename FactorizedPolynomial<P>::CoeffType& _rhs )
    {
        return !existsFactorization( _lhs ) && _lhs.coefficient() == _rhs;
    }
    
    template<typename P>
    bool operator<( const FactorizedPolynomial<P>& _lhs, const typename P::CoeffType& _rhs )
    {
        return !existsFactorization( _lhs ) && _lhs.coefficient() < _rhs;
    }
        
    template<typename P>
    bool operator<( const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs )
    {
        ASSERT_CACHE_EQUAL( _lhs.pCache(), _rhs.pCache() );
        
        if( _lhs.pCache() == nullptr && _rhs.pCache() == nullptr )
        {
            return _lhs.coefficient() < _rhs.coefficient();
        }
        else if( _lhs.pCache() != nullptr && _rhs.pCache() != nullptr )
        {
            if( _lhs.content() == _rhs.content() )
                return _lhs.coefficient() < _rhs.coefficient(); 
            return _lhs.content() < _rhs.content();
        }
        return _lhs.pCache() == nullptr;
    }
    
    template<typename P>
    bool operator!=( const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs )
    {
        ASSERT_CACHE_EQUAL( _lhs.pCache(), _rhs.pCache() );
        if( _lhs.pCache() == nullptr && _rhs.pCache() == nullptr )
        {
            return _lhs.coefficient() != _rhs.coefficient();
        }
        else if( _lhs.pCache() != nullptr && _rhs.pCache() != nullptr )
        {
            return !(_lhs.content() == _rhs.content());
        }
        return true;
    }
    
    template<typename P>
    P computePolynomial( const FactorizedPolynomial<P>& _fpoly )
    {
        if( _fpoly.pCache() == nullptr )
            return std::move( P( _fpoly.coefficient() ) );
        P result = computePolynomial( _fpoly.content() );
        result *= _fpoly.coefficient();
        return result;
    }
    
    template<typename P>
    Coeff<P> distributeCoefficients( Factorization<P>& _factorization )
    {
        Coeff<P> result(1);
        for ( auto factor = _factorization.begin(); factor != _factorization.end(); factor++ )
        {
            result *= carl::pow( factor->first.coefficient(), factor->second );
            factor->first.mCoefficient = 1;
        }
        return std::move( result );
    }

    template<typename P>
    FactorizedPolynomial<P> FactorizedPolynomial<P>::operator-() const
    {
        FactorizedPolynomial<P> result( *this );
        result.mCoefficient *= Coeff<P>( -1 );
        assert( (-computePolynomial( *this )) == computePolynomial( result ) );
        return std::move( result );
    }

    template<typename P>
    FactorizedPolynomial<P> operator+( const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs )
    {
        ASSERT_CACHE_EQUAL( _lhs.pCache(), _rhs.pCache() );
        _lhs.strengthenActivity();
        _rhs.strengthenActivity();
        
        // Handle cases where one or both are constant
        if( !existsFactorization( _lhs ) )
        {
            if( existsFactorization( _rhs ) )
            {
                FactorizedPolynomial<P> result( _rhs.polynomial() * _rhs.coefficient() + _lhs.coefficient(), _rhs.pCache() );
                assert( computePolynomial( _lhs ) + computePolynomial( _rhs ) == computePolynomial( result ) );
                return std::move( result );
            }
            else
            {
                FactorizedPolynomial<P> result( _lhs.coefficient() + _rhs.coefficient() );
                assert( computePolynomial( _lhs ) + computePolynomial( _rhs ) == computePolynomial( result ) );
                return std::move( result );
            }
        }
        else if( !existsFactorization( _rhs ) )
        {
            FactorizedPolynomial<P> result( _lhs.polynomial() * _lhs.coefficient() + _rhs.coefficient(), _lhs.pCache() );
            assert( computePolynomial( _lhs ) + computePolynomial( _rhs ) == computePolynomial( result ) );
            return std::move( result );
        }

        Coeff<P> coefficientCommon = Coeff<P>(carl::gcd( carl::getNum( _lhs.coefficient() ), carl::getNum( _rhs.coefficient() ) )) /
                Coeff<P>(carl::lcm( carl::getDenom( _lhs.coefficient() ), carl::getDenom( _rhs.coefficient() ) ));
        Coeff<P> coefficientRestA = _lhs.coefficient() / coefficientCommon;
        Coeff<P> coefficientRestB = _rhs.coefficient() / coefficientCommon;

        if (carl::isZero(coefficientCommon))
        {
            FactorizedPolynomial<P> result;
            assert( computePolynomial( _lhs ) + computePolynomial( _rhs ) == computePolynomial( result ) );
            return std::move( result );
        }

        Factorization<P> factorizationRestA, factorizationRestB;
        assert( existsFactorization( _lhs ) );
        const Factorization<P>& factorizationA = _lhs.factorization();
        assert( existsFactorization( _rhs ) );
        const Factorization<P>& factorizationB = _rhs.factorization();

        //Compute common divisor as factor of result
        Factorization<P> resultFactorization = commonDivisor( factorizationA, factorizationB, factorizationRestA, factorizationRestB );

        //Compute remaining sum
        P sum;
        if( resultFactorization.empty() )
        {
            sum = _lhs.polynomial() * coefficientRestA;
            sum += _rhs.polynomial() * coefficientRestB;
        }
        else
        {
            sum = computePolynomial( factorizationRestA ) * coefficientRestA;
            sum += computePolynomial( factorizationRestB ) * coefficientRestB;
        }
        if ( carl::isZero(sum) )
        {
            FactorizedPolynomial<P> result;
            assert( computePolynomial( _lhs ) + computePolynomial( _rhs ) == computePolynomial( result ) );
            return std::move( result );
        }
        else
        {
            if ( sum.isConstant() )
                coefficientCommon *= sum.constantPart();
            else
            {
                FactorizedPolynomial<P> fpolySum( sum, _lhs.pCache() );
                coefficientCommon *= fpolySum.coefficient();
                fpolySum.mCoefficient = Coeff<P>(1);
                resultFactorization.insert( std::pair<FactorizedPolynomial<P>, carl::exponent>( fpolySum, 1 ) );
            }
        }
        FactorizedPolynomial<P> result( std::move( resultFactorization ), coefficientCommon, FactorizedPolynomial<P>::chooseCache( _lhs.pCache(), _rhs.pCache() ) );
        assert( computePolynomial( _lhs ) + computePolynomial( _rhs ) == computePolynomial( result ) );
        return std::move( result );
    }
    
    template<typename P>
    FactorizedPolynomial<P> operator+( const FactorizedPolynomial<P>& _fpoly, const typename FactorizedPolynomial<P>::CoeffType& _coef )
    {
        FactorizedPolynomial<P> result( _fpoly );
        return std::move( result += _coef ); 
    }

    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator+=( const CoeffType& _coef )
    {
        FactorizedPolynomial<P> result = *this + FactorizedPolynomial<P>( _coef );
        return *this = result;
    }

    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator+=( const FactorizedPolynomial<P>& _fpoly )
    {
        FactorizedPolynomial<P> result = *this + _fpoly;
        return *this = result;
    }
    
    template<typename P>
    FactorizedPolynomial<P> operator-( const FactorizedPolynomial<P>& _fpoly, const typename FactorizedPolynomial<P>::CoeffType& _coef )
    {
        FactorizedPolynomial<P> result( _fpoly );
        return std::move( result -= _coef ); 
    }

    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator-=( const CoeffType& _coef )
    {
        FactorizedPolynomial<P> result = *this + FactorizedPolynomial<P>( CoeffType( -1 ) * _coef );
        assert( computePolynomial( *this ) - _coef == computePolynomial( result ) );
        return *this = result;
    }

    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator-=( const FactorizedPolynomial<P>& _fpoly )
    {
        FactorizedPolynomial<P> result = *this - _fpoly;
        return *this = result;
    }

    template<typename P>
    FactorizedPolynomial<P> operator-( const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs )
    {
        ASSERT_CACHE_EQUAL( _lhs.pCache(), _rhs.pCache() );
        const Coeff<P>& coefficient = -_rhs.coefficient();
        if ( existsFactorization( _rhs ) )
        {
            FactorizedPolynomial<P> result = _lhs + FactorizedPolynomial<P>( std::move( Factorization<P>( _rhs.factorization() ) ), coefficient, _rhs.pCache() );
            assert( computePolynomial( _lhs ) - computePolynomial( _rhs ) == computePolynomial( result ) );
            return std::move( result );
        }
        else
        {
            FactorizedPolynomial<P> result = _lhs + FactorizedPolynomial<P>( coefficient );
            assert( computePolynomial( _lhs ) - computePolynomial( _rhs ) == computePolynomial( result ) );
            return std::move( result );
        }
    }
    
    template<typename P>
    FactorizedPolynomial<P> operator*( const FactorizedPolynomial<P>& _fpoly, const typename FactorizedPolynomial<P>::CoeffType& _coeff )
    {
        if( carl::isZero( _coeff ) )
        {
            FactorizedPolynomial<P> result;
            assert( computePolynomial( _fpoly ) * _coeff == computePolynomial( result ) );
            return std::move( result );
        }
        FactorizedPolynomial<P> result( _fpoly );
        result.mCoefficient *= _coeff;
        assert( computePolynomial( _fpoly ) * _coeff == computePolynomial( result ) );
        return std::move( result );
    }

    template<typename P>
    FactorizedPolynomial<P> operator*( const FactorizedPolynomial<P>& _lhs, const FactorizedPolynomial<P>& _rhs )
    {
        ASSERT_CACHE_EQUAL( _lhs.pCache(), _rhs.pCache() );
        _lhs.strengthenActivity();
        _rhs.strengthenActivity();

        if( _lhs.isZero() || _rhs.isZero() )
        {
            FactorizedPolynomial<P> result;
            assert( computePolynomial( _lhs ) * computePolynomial( _rhs ) == computePolynomial( result ) );
            return std::move( result );
        }

        if( !existsFactorization( _rhs ) )
        {
            FactorizedPolynomial<P> result( _lhs );
            result.mCoefficient *= _rhs.mCoefficient;
            assert( computePolynomial( _lhs ) * computePolynomial( _rhs ) == computePolynomial( result ) );
            return std::move( result );
        }
        else if( !existsFactorization( _lhs ) )
        {
            FactorizedPolynomial<P> result( _rhs );
            result.mCoefficient *= _lhs.mCoefficient;
            assert( computePolynomial( _lhs ) * computePolynomial( _rhs ) == computePolynomial( result ) );
            return std::move( result );
        }

        Factorization<P> resultFactorization;
        const Factorization<P>& factorizationA = _lhs.factorization();
        const Factorization<P>& factorizationB = _rhs.factorization();
        auto factorA = factorizationA.begin();
        auto factorB = factorizationB.begin();
        while( factorA != factorizationA.end() && factorB != factorizationB.end() )
        {
            if( factorA->first == factorB->first )
            {
                resultFactorization.insert( resultFactorization.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>(factorA->first, factorA->second + factorB->second ) );
                factorA++;
                factorB++;
            }
            else if( factorA->first < factorB->first )
            {
                resultFactorization.insert( resultFactorization.end(), *factorA );
                factorA++;
            }
            else
            {
                resultFactorization.insert( resultFactorization.end(), *factorB );
                factorB++;
            }
        }
        while ( factorA != factorizationA.end() )
        {
            resultFactorization.insert( resultFactorization.end(), *factorA );
            factorA++;
        }
        while ( factorB != factorizationB.end() )
        {
            resultFactorization.insert( resultFactorization.end(), *factorB );
            factorB++;
        }

        Coeff<P> coefficientResult = _lhs.coefficient() * _rhs.coefficient();
        //TODO needed?
        coefficientResult *= distributeCoefficients( resultFactorization );
        FactorizedPolynomial<P> result( std::move( resultFactorization ), coefficientResult, FactorizedPolynomial<P>::chooseCache( _lhs.pCache(), _rhs.pCache() ) );
        assert( computePolynomial( _lhs ) * computePolynomial( _rhs ) == computePolynomial( result ) );
        return std::move( result );
    }
    
    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator*=( const CoeffType& _coef )
    {
        if( carl::isZero( _coef ) && mpCache != nullptr )
        {
            mpCache->dereg( mCacheRef );
            mCacheRef = CACHE::NO_REF;
            mpCache = nullptr;
        }
        mCoefficient *= _coef;
        ASSERT_CACHE_REF_LEGAL( (*this) );
        return *this;
    }
    
    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator*=( const FactorizedPolynomial<P>& _fpoly )
    {
        FactorizedPolynomial<P> result = *this * _fpoly;
        return *this = result;
    }

    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator/=( const CoeffType& _coef )
    {
        assert( !carl::isZero( _coef ) );
        FactorizedPolynomial<P> tmp = *this;
        this->mCoefficient /= _coef;
        assert(computePolynomial(tmp) * (static_cast<CoeffType>(1)/_coef) == computePolynomial(*this));
        return *this;
    }
    
    template<typename P>
    FactorizedPolynomial<P>& FactorizedPolynomial<P>::operator/=( const FactorizedPolynomial<P>& _fpoly )
    {
        assert( !_fpoly.isZero() );
        FactorizedPolynomial<P> result = this->quotient( _fpoly );
        assert( computePolynomial( *this ) / computePolynomial( _fpoly ) == computePolynomial( result ) );
        return *this = result;
    }

    template<typename P>
    FactorizedPolynomial<P> FactorizedPolynomial<P>::quotient(const FactorizedPolynomial<P>& _fdivisor) const
    {
        assert( !_fdivisor.isZero() );
        if( isZero() )
        {
            FactorizedPolynomial<P> result;
            assert( carl::quotient(computePolynomial( *this ), computePolynomial( _fdivisor ) ) == computePolynomial( result ) );
            return std::move( result );
        }
        FactorizedPolynomial<P> result = lazyDiv( *this, _fdivisor ).first;
        assert( carl::quotient(computePolynomial( *this ), computePolynomial( _fdivisor ) ) == computePolynomial( result ) );
        return std::move( result );
    }
    
    template<typename P>
    std::string FactorizedPolynomial<P>::toString( bool _infix, bool _friendlyVarNames ) const
    {
        if( existsFactorization( *this ) )
        {
            std::stringstream result;
            if( _infix )
            {
                if( mCoefficient != Coeff<P>( 1 ) )
                {
                    result << mCoefficient << " * (";
                }
                result << content();
                if( mCoefficient != Coeff<P>( 1 ) )
                    result << ")";
            }
            else
            {
                bool withCoeff = mCoefficient != Coeff<P>( 1 );
                if( withCoeff )
                {
                    result << "(* " << mCoefficient << " ";
                }
                result << content();
                if( withCoeff )
                    result << ")";
            }
            return result.str();
        }
        std::stringstream s;
        s << mCoefficient;
        return s.str();
    }

    template<typename P>
    FactorizedPolynomial<P> quotient( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyB.isZero() );
        return std::move( _fpolyA.quotient( _fpolyB ) );
    }

    template<typename P>
    std::pair<FactorizedPolynomial<P>,FactorizedPolynomial<P>> lazyDiv( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyB.isZero() );
        if( _fpolyA.isZero() )
            return std::move( std::make_pair( FactorizedPolynomial<P>(), FactorizedPolynomial<P>( Coeff<P>( 1 ) ) ) );
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        // Handle cases where one or both are constant
        if( !existsFactorization( _fpolyB ) )
        {
            FactorizedPolynomial<P> fPolyASimplified( _fpolyA );
            assert( !carl::isZero(_fpolyB.mCoefficient) );
            fPolyASimplified.mCoefficient /= _fpolyB.mCoefficient;
            auto result = std::make_pair( fPolyASimplified, FactorizedPolynomial<P>( Coeff<P>( 1 ) ) );
            assert( computePolynomial( result.first ) * computePolynomial( _fpolyB ) == computePolynomial( result.second ) * computePolynomial( _fpolyA ) );
            return std::move( result );
        }
        else if( !existsFactorization( _fpolyA ) )
        {
            FactorizedPolynomial<P> fPolyASimplified( _fpolyA );
            assert( !carl::isZero(_fpolyB.mCoefficient) );
            fPolyASimplified.mCoefficient /= _fpolyB.mCoefficient;
            FactorizedPolynomial<P> fPolyBSimplified( _fpolyB );
            assert( !carl::isZero(_fpolyA.mCoefficient) );
            fPolyBSimplified.mCoefficient = Coeff<P>( 1 );
            auto result = std::make_pair( fPolyASimplified, fPolyBSimplified );
            assert( computePolynomial( result.first ) * computePolynomial( _fpolyB ) == computePolynomial( result.second ) * computePolynomial( _fpolyA ) );
            return std::move( result );
        }
        Factorization<P> resultFactorizationA;
        Factorization<P> resultFactorizationB;
        const Factorization<P>& factorizationA = _fpolyA.factorization();
        const Factorization<P>& factorizationB = _fpolyB.factorization();
        auto factorA = factorizationA.begin();
        auto factorB = factorizationB.begin();
        while( factorA != factorizationA.end() && factorB != factorizationB.end() )
        {
            if( factorA->first == factorB->first )
            {
                if( factorA->second > factorB->second )
                    resultFactorizationA.insert( resultFactorizationA.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>(factorA->first, factorA->second - factorB->second ) );
                else if( factorA->second < factorB->second )
                    resultFactorizationB.insert( resultFactorizationB.end(), std::pair<FactorizedPolynomial<P>, carl::exponent>(factorA->first, factorB->second - factorA->second ) );
                factorA++;
                factorB++;
            }
            else if( factorA->first < factorB->first )
            {
                resultFactorizationA.insert( resultFactorizationA.end(), *factorA );
                factorA++;
            }
            else
            {
                resultFactorizationB.insert( resultFactorizationB.end(), *factorB );
                factorB++;
            }
        }
        while ( factorA != factorizationA.end() )
        {
            resultFactorizationA.insert( resultFactorizationA.end(), *factorA );
            factorA++;
        }
        while ( factorB != factorizationB.end() )
        {
            resultFactorizationB.insert( resultFactorizationB.end(), *factorB );
            factorB++;
        }
        Coeff<P> coefficientResultA = _fpolyA.coefficient() / _fpolyB.coefficient();
        auto cache = FactorizedPolynomial<P>::chooseCache( _fpolyA.pCache(), _fpolyB.pCache() );
        FactorizedPolynomial<P> resultA( std::move( resultFactorizationA ), coefficientResultA, cache );
        FactorizedPolynomial<P> resultB( std::move( resultFactorizationB ), Coeff<P>( 1 ), cache );
        assert( computePolynomial( resultA ) * computePolynomial( _fpolyB ) == computePolynomial( resultB ) * computePolynomial( _fpolyA ) );
        return std::make_pair( resultA, resultB );
    }

    template<typename P>
    FactorizedPolynomial<P> lcm( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyA.isZero() && !_fpolyB.isZero() );
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        bool rehashFPolyA = false;
        bool rehashFPolyB = false;
        Coeff<P> coefficientLCM = Coeff<P>(carl::lcm( carl::getNum(_fpolyA.coefficient()), carl::getNum(_fpolyB.coefficient()) )) /
                Coeff<P>(carl::gcd( carl::getDenom(_fpolyA.coefficient()), carl::getDenom(_fpolyB.coefficient()) ));
        // Handle cases where one or both are constant
        if( !existsFactorization( _fpolyB ) )
        {
            FactorizedPolynomial<P> result( _fpolyA );
            result.mCoefficient = coefficientLCM;
            assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyA ) )) );
            assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyB ) )) );
            return std::move( result );
        }
        else if( !existsFactorization( _fpolyA ) )
        {
            FactorizedPolynomial<P> result( _fpolyB );
            result.mCoefficient = coefficientLCM;
            assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyA ) )) );
            assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyB ) )) );
            return std::move( result );
        }
        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "Compute LCM of " << _fpolyA << " and " << _fpolyB );

        //Both polynomials are not constant
        Factorization<P> restAFactorization, restBFactorization;
        Coeff<P> c( 0 );
        gcd( _fpolyA.content(), _fpolyB.content(), restAFactorization, restBFactorization, c, rehashFPolyA, rehashFPolyB );
        if( c != Coeff<P>( 0 ) )
            coefficientLCM *= c;

        if( rehashFPolyA )
            _fpolyA.rehash();
        if( rehashFPolyB )
            _fpolyB.rehash();

        //Compute lcm as A*restB
        Factorization<P> lcmFactorization;
        if( existsFactorization( _fpolyA ) )
            lcmFactorization.insert( _fpolyA.factorization().begin(), _fpolyA.factorization().end() );
        for ( auto factor = restBFactorization.begin(); factor != restBFactorization.end(); factor++ )
        {
            lcmFactorization.insert( *factor );
        }

        coefficientLCM *= distributeCoefficients( lcmFactorization );
        FactorizedPolynomial<P> result( std::move( lcmFactorization ), coefficientLCM, _fpolyA.pCache() );
        CARL_LOG_DEBUG( "carl.core.factorizedpolynomial", "LCM of " << _fpolyA << " and " << _fpolyB << ": " << result);
        assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyA ) )) );
        assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyB ) )) );
        return std::move( result );
    }

    template<typename P>
    FactorizedPolynomial<P> commonMultiple( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyA.isZero() && !_fpolyB.isZero() );
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        
        Coeff<P> coefficientLCM = Coeff<P>(carl::lcm( carl::getNum(_fpolyA.coefficient()), carl::getNum(_fpolyB.coefficient()) )) /
                Coeff<P>(carl::gcd( carl::getDenom(_fpolyA.coefficient()), carl::getDenom(_fpolyB.coefficient()) ));
        // Handle cases where one or both are constant
        if( !existsFactorization( _fpolyB ) )
        {
            FactorizedPolynomial<P> result( _fpolyA );
            result.mCoefficient = coefficientLCM;
            assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyA ) )) );
            assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyB ) )) );
            return std::move( result );
        }
        else if( !existsFactorization( _fpolyA ) )
        {
            FactorizedPolynomial<P> result( _fpolyB );
            result.mCoefficient = coefficientLCM;
            assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyA ) )) );
            assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyB ) )) );
            return std::move( result );
        }
        
        Factorization<P> cmFactorization;
        const Factorization<P>& factorizationA = _fpolyA.factorization();
        const Factorization<P>& factorizationB = _fpolyB.factorization();
        auto factorA = factorizationA.begin();
        auto factorB = factorizationB.begin();
        while( factorA != factorizationA.end() && factorB != factorizationB.end() )
        {
            if( factorA->first == factorB->first )
            {
                cmFactorization.insert( cmFactorization.end(), factorA->second > factorB->second ? *factorA : *factorB );
                factorA++;
                factorB++;
            }
            else if( factorA->first < factorB->first )
            {
                cmFactorization.insert( cmFactorization.end(), *factorA );
                factorA++;
            }
            else
            {
                cmFactorization.insert( cmFactorization.end(), *factorB );
                factorB++;
            }
        }
        while ( factorA != factorizationA.end() )
        {
            cmFactorization.insert( cmFactorization.end(), *factorA );
            factorA++;
        }
        while ( factorB != factorizationB.end() )
        {
            cmFactorization.insert( cmFactorization.end(), *factorB );
            factorB++;
        }
        FactorizedPolynomial<P> result( std::move( cmFactorization ), coefficientLCM, FactorizedPolynomial<P>::chooseCache( _fpolyA.pCache(), _fpolyB.pCache() ) );
        assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyA ) )) );
        assert( carl::isZero(carl::remainder(computePolynomial( result ), computePolynomial( _fpolyB ) )) );
        return std::move( result );
    }

    template<typename P>
    FactorizedPolynomial<P> commonDivisor( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyA.isZero() && !_fpolyB.isZero() );
        ASSERT_CACHE_EQUAL( _fpolyA.pCache(), _fpolyB.pCache() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();
        Coeff<P> coefficientCommon = Coeff<P>(carl::gcd( carl::getNum( _fpolyA.coefficient() ), carl::getNum( _fpolyB.coefficient() ) )) /
                Coeff<P>(carl::lcm( carl::getDenom( _fpolyA.coefficient() ), carl::getDenom( _fpolyB.coefficient() ) ));
        // Handle cases where one or both are constant
        if( !existsFactorization( _fpolyA ) || !existsFactorization( _fpolyB ) )
        {
            FactorizedPolynomial<P> result( coefficientCommon );
            assert( carl::isZero(carl::remainder(computePolynomial( _fpolyA ), computePolynomial( result ) )) );
            assert( carl::isZero(carl::remainder(computePolynomial( _fpolyB ), computePolynomial( result ) )) );
            return std::move( result );
        }
        
        Factorization<P> cdFactorization;
        const Factorization<P>& factorizationA = _fpolyA.factorization();
        const Factorization<P>& factorizationB = _fpolyB.factorization();
        auto factorA = factorizationA.begin();
        auto factorB = factorizationB.begin();
        while( factorA != factorizationA.end() && factorB != factorizationB.end() )
        {
            if( factorA->first == factorB->first )
            {
                cdFactorization.insert( cdFactorization.end(), factorA->second < factorB->second ? *factorA : *factorB );
                factorA++;
                factorB++;
            }
            else if( factorA->first < factorB->first )
                factorA++;
            else
                factorB++;
        }
        FactorizedPolynomial<P> result( std::move( cdFactorization ), coefficientCommon, FactorizedPolynomial<P>::chooseCache( _fpolyA.pCache(), _fpolyB.pCache() ) );
        assert( carl::isZero(carl::remainder(computePolynomial( _fpolyA ), computePolynomial( result ) )) );
        assert( carl::isZero(carl::remainder(computePolynomial( _fpolyB ), computePolynomial( result ) )) );
        return std::move( result );
    }

    template<typename P>
    Factorization<P> commonDivisor( const Factorization<P>& _fFactorizationA, const Factorization<P>& _fFactorizationB, Factorization<P>& _fFactorizationRestA, Factorization<P>& _fFactorizationRestB)
    {
        assert( !_fFactorizationA.empty() && !_fFactorizationB.empty() );
        Factorization<P> resultFactorization;
        _fFactorizationRestA.clear();
        _fFactorizationRestB.clear();
        auto factorA = _fFactorizationA.begin();
        auto factorB = _fFactorizationB.begin();
        while( factorA != _fFactorizationA.end() && factorB != _fFactorizationB.end() )
        {
            if( factorA->first == factorB->first )
            {
                if( factorA->second < factorB->second )
                {
                    resultFactorization.insert( resultFactorization.end(), *factorA );
                    _fFactorizationRestB.insert( _fFactorizationRestB.end(), std::make_pair( factorB->first, factorB->second - factorA->second ) );
                }
                else if( factorA->second > factorB->second )
                {
                    resultFactorization.insert( resultFactorization.end(), *factorB );
                    _fFactorizationRestA.insert( _fFactorizationRestA.end(), std::make_pair( factorA->first, factorA->second - factorB->second ) );
                }
                else
                {
                    resultFactorization.insert( resultFactorization.end(), *factorA );
                }
                factorA++;
                factorB++;
            }
            else if( factorA->first < factorB->first )
            {
                _fFactorizationRestA.insert( _fFactorizationRestA.end(), *factorA );
                factorA++;
            }
            else
            {
                _fFactorizationRestB.insert( _fFactorizationRestB.end(), *factorB );
                factorB++;
            }
        }
        while ( factorA != _fFactorizationA.end() )
        {
            _fFactorizationRestA.insert( _fFactorizationRestA.end(), *factorA );
            factorA++;
        }
        while ( factorB != _fFactorizationB.end() )
        {
            _fFactorizationRestB.insert( _fFactorizationRestB.end(), *factorB );
            factorB++;
        }
        assert( computePolynomial( _fFactorizationA ) == computePolynomial( resultFactorization ) * computePolynomial( _fFactorizationRestA ) );
        assert( computePolynomial( _fFactorizationB ) == computePolynomial( resultFactorization ) * computePolynomial( _fFactorizationRestB ) );
        return std::move( resultFactorization );
    }

    template<typename P>
    FactorizedPolynomial<P> gcd( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB )
    {
        assert( !_fpolyA.isZero() && !_fpolyB.isZero() );
        FactorizedPolynomial<P> restA, restB;
        return std::move( gcd( _fpolyA, _fpolyB, restA, restB ) );
    }

    template<typename P>
    FactorizedPolynomial<P> gcd( const FactorizedPolynomial<P>& _fpolyA, const FactorizedPolynomial<P>& _fpolyB, FactorizedPolynomial<P>& _fpolyRestA, FactorizedPolynomial<P>& _fpolyRestB )
    {
        assert( !_fpolyA.isZero() && !_fpolyB.isZero() );
        _fpolyA.strengthenActivity();
        _fpolyB.strengthenActivity();

        Coeff<P> coefficientCommon = Coeff<P>(carl::gcd( carl::getNum( _fpolyA.coefficient() ), carl::getNum( _fpolyB.coefficient() ) )) /
        Coeff<P>(carl::lcm( carl::getDenom( _fpolyA.coefficient() ), carl::getDenom( _fpolyB.coefficient() ) ));
        Coeff<P> coefficientRestA = _fpolyA.coefficient() / coefficientCommon;
        Coeff<P> coefficientRestB = _fpolyB.coefficient() / coefficientCommon;
        
        //Handle cases where one or both are constant
        if ( !existsFactorization( _fpolyA ) )
        {
            if ( existsFactorization( _fpolyB ) )
            {
                _fpolyRestA = FactorizedPolynomial<P>( coefficientRestA );
                _fpolyRestB = FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyB.factorization() ) ), coefficientRestB, _fpolyB.pCache() );
                FactorizedPolynomial<P> result( coefficientCommon );
                assert( computePolynomial( _fpolyA ) == computePolynomial( result ) * computePolynomial( _fpolyRestA ) );
                assert( computePolynomial( _fpolyB ) == computePolynomial( result ) * computePolynomial( _fpolyRestB ) );
                return std::move( result );
            }
            else
            {
                _fpolyRestA = FactorizedPolynomial<P>( coefficientRestA );
                _fpolyRestB = FactorizedPolynomial<P>( coefficientRestB );
                FactorizedPolynomial<P> result( coefficientCommon );
                assert( computePolynomial( _fpolyA ) == computePolynomial( result ) * computePolynomial( _fpolyRestA ) );
                assert( computePolynomial( _fpolyB ) == computePolynomial( result ) * computePolynomial( _fpolyRestB ) );
                return std::move( result );
            }
        }
        else if ( !existsFactorization( _fpolyB ) )
        {
            _fpolyRestA = FactorizedPolynomial<P>( std::move( Factorization<P>( _fpolyA.factorization() ) ), coefficientRestA, _fpolyA.pCache());
            _fpolyRestB = FactorizedPolynomial<P>( coefficientRestB );
            FactorizedPolynomial<P> result( coefficientCommon );
            assert( computePolynomial( _fpolyA ) == computePolynomial( result ) * computePolynomial( _fpolyRestA ) );
            assert( computePolynomial( _fpolyB ) == computePolynomial( result ) * computePolynomial( _fpolyRestB ) );
            return std::move( result );
        }

        //Both polynomials are not constant
        Factorization<P> restAFactorization, restBFactorization;
        Coeff<P> c( 0 );
        bool rehashFPolyA = false;
        bool rehashFPolyB = false;
        Factorization<P> gcdFactorization = gcd( _fpolyA.content(), _fpolyB.content(), restAFactorization, restBFactorization, c, rehashFPolyA, rehashFPolyB );

        if( c != Coeff<P>( 0 ) )
            coefficientCommon *= c;
        if( rehashFPolyA )
            _fpolyA.rehash();
        if( rehashFPolyB )
            _fpolyB.rehash();

        coefficientRestA *= distributeCoefficients( restAFactorization );
        coefficientRestB *= distributeCoefficients( restBFactorization );
        coefficientCommon *= distributeCoefficients( gcdFactorization );
        _fpolyRestA = FactorizedPolynomial<P>( std::move( restAFactorization ), coefficientRestA, _fpolyA.pCache());
        _fpolyRestB = FactorizedPolynomial<P>( std::move( restBFactorization ), coefficientRestB, _fpolyA.pCache());
        
        FactorizedPolynomial<P> result( std::move( gcdFactorization ), coefficientCommon, _fpolyA.pCache() );
        assert( computePolynomial( _fpolyA ) == computePolynomial( result ) * computePolynomial( _fpolyRestA ) );
        assert( computePolynomial( _fpolyB ) == computePolynomial( result ) * computePolynomial( _fpolyRestB ) );
        return std::move( result );
    }
    
    template<typename P>
    Factors<FactorizedPolynomial<P>> factor( const FactorizedPolynomial<P>& _fpoly )
    {
        return factor( _fpoly.content(), _fpoly.coefficient() );
    }
    
    template <typename P>
    std::ostream& operator<<( std::ostream& _out, const FactorizedPolynomial<P>& _fpoly )
    {
        _out << _fpoly.toString();
        return _out;
    }
} // namespace carl
