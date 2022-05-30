/**
 * Class to create a square root expression object.
 * @author Florian Corzilius
 * @since 2011-05-26
 * @version 2013-10-07
 */

#include <carl/poly/umvpoly/functions/GCD.h>

namespace carl
{

	template<typename Poly>
    SqrtEx<Poly>::SqrtEx():
        m_constant_part(),
        m_factor(),
        m_denominator(1),
        m_radicand()
    {}

	template<typename Poly>
    SqrtEx<Poly>::SqrtEx( Poly&& _poly ):
        m_constant_part( std::move( _poly ) ),
        m_factor(),
        m_denominator(1),
        m_radicand()
    {
        normalize();
    }

	template<typename Poly>
    SqrtEx<Poly>::SqrtEx( Poly&& _constantPart, Poly&& _factor, Poly&& _denominator, Poly&& _radicand ):
        m_constant_part( std::move( _constantPart ) ),
        m_factor( is_zero(_radicand) ? std::move( _radicand ) : std::move( _factor ) ),
        m_denominator( (is_zero(m_factor) && is_zero(m_constant_part)) ? constant_one<Poly>::get() : std::move( _denominator ) ),
        m_radicand( is_zero(m_factor) ? m_factor : std::move( _radicand ) )
    {
        assert( !is_zero(m_denominator) );
        assert( !m_radicand.is_constant() || is_zero(m_radicand) || constant_zero<Rational>::get() <= m_radicand.trailingTerm().coeff() );
        normalize();
    }

	template<typename Poly>
    void SqrtEx<Poly>::normalize()
    {
//        std::cout << *this << std::endl;
        Poly gcdA;
        if( is_zero(m_factor) )
        {
            gcdA = m_constant_part;
        }
        else 
        {
            Poly sqrtOfRadicand;
            if( m_radicand.sqrt( sqrtOfRadicand ) )
            {
                m_constant_part += m_factor * sqrtOfRadicand;
                m_factor = constant_zero<Poly>::get();
                m_radicand = constant_zero<Poly>::get();
            }
            else
            {
                assert( !is_zero(radicand()) );
                Rational absOfLCoeff = abs( radicand().coprime_factor() );
                Rational sqrtResult = 0;
                if( carl::sqrt_exact( absOfLCoeff, sqrtResult ) )
                {
                    m_factor *= constant_one<Rational>::get()/sqrtResult;
                    m_radicand *= absOfLCoeff;
                }
            }
            if( is_zero(m_factor) )
            {
                gcdA = m_constant_part;
            }
            else
            {
                if( is_zero(m_constant_part) )
                {
                    gcdA = m_factor;
                }
                else
                {
                    Rational ccConstantPart = m_constant_part.coprime_factor();
                    Poly cpConstantPart = m_constant_part * ccConstantPart;
                    Rational ccFactor = m_factor.coprime_factor();
                    Poly cpFactor = m_factor * ccFactor;
                    gcdA = carl::gcd( cpConstantPart, cpFactor )*carl::gcd(ccConstantPart,ccFactor);
                }
            }
        }
        if( is_zero(gcdA) ) return;
        Rational ccGcdA = gcdA.coprime_factor();
        Poly cpGcdA = gcdA * ccGcdA;
        Rational ccDenominator = m_denominator.coprime_factor();
        Poly cpDenominator = m_denominator * ccDenominator;
        gcdA = carl::gcd( cpGcdA, cpDenominator )*carl::gcd(ccGcdA,ccDenominator);
        // Make sure that the polynomial to divide by cannot be negative, otherwise the sign of the square root expression could change.
        if( !(gcdA == constant_one<Poly>::get()) && carl::definiteness(gcdA) == carl::Definiteness::POSITIVE_SEMI )
        {
            if( !is_zero(m_constant_part) )
            {
				carl::try_divide(m_constant_part, gcdA, m_constant_part );
            }
            if( !is_zero(m_factor) )
            {
                carl::try_divide(m_factor, gcdA, m_factor );
            }
            carl::try_divide(m_denominator, gcdA, m_denominator );
        }
        Rational numGcd = constant_zero<Rational>::get();
        Rational denomLcm = constant_one<Rational>::get();
        if( is_zero(factor()) )
        {
            if( !is_zero(constant_part()) )
            {
                Rational cpOfConstantPart = constant_part().coprime_factor();
                numGcd = carl::get_num( cpOfConstantPart );
                denomLcm = carl::get_denom( cpOfConstantPart );
            }
            else
            {
//                std::cout << "        to " << *this << std::endl;
                return; // Sqrt expression corresponds to 0.
            }
        }
        else
        {
            Rational cpOfFactorPart = factor().coprime_factor();
            if( is_zero(constant_part()) )
            {
                numGcd = carl::get_num( cpOfFactorPart );
                denomLcm = carl::get_denom( cpOfFactorPart );
            }
            else
            {
                Rational cpOfConstantPart = constant_part().coprime_factor();
                numGcd = carl::gcd( carl::get_num( cpOfConstantPart ), carl::get_num( cpOfFactorPart ) );
                denomLcm = carl::lcm( carl::get_denom( cpOfConstantPart ), carl::get_denom( cpOfFactorPart ) );
            }
        }
        assert( numGcd != constant_zero<Rational>::get() );
        Rational cpFactor = numGcd/denomLcm; 
        m_constant_part *= cpFactor;
        m_factor *= cpFactor;
        Rational cpOfDenominator = denominator().coprime_factor();
        m_denominator *= cpOfDenominator;
        Rational sqrtExFactor = (denomLcm*carl::get_num( cpOfDenominator ))/(numGcd*carl::get_denom( cpOfDenominator ));
        m_constant_part *= carl::get_num( sqrtExFactor );
        m_factor *= carl::get_num( sqrtExFactor );
        m_denominator *= carl::get_denom( sqrtExFactor );
//        std::cout << "       to  " << *this << std::endl;
        //TODO: implement this method further
    }

	template<typename Poly>
    bool SqrtEx<Poly>::operator==( const SqrtEx& _toCompareWith ) const
    {
        return    m_radicand == _toCompareWith.radicand() && m_denominator == _toCompareWith.denominator() 
               && m_factor == _toCompareWith.factor() && m_constant_part == _toCompareWith.constant_part();
    }

	template<typename Poly>
    SqrtEx<Poly>& SqrtEx<Poly>::operator=( const SqrtEx<Poly>& _sqrtEx )
    {
        if( this != &_sqrtEx )
        {
            m_constant_part = _sqrtEx.constant_part();
            m_factor       = _sqrtEx.factor();
            m_denominator  = _sqrtEx.denominator();
            if (is_zero(factor()))
                m_radicand = constant_zero<Poly>::get();
            else
                m_radicand = _sqrtEx.radicand();
        }
        return *this;
    }

	template<typename Poly>
    SqrtEx<Poly>& SqrtEx<Poly>::operator=( const Poly& _poly )
    {
        m_constant_part = _poly;
        m_factor       = constant_zero<Poly>::get();
        m_denominator  = constant_one<Poly>::get();
        m_radicand     = constant_zero<Poly>::get();
        return *this;
    }

	template<typename Poly>
    SqrtEx<Poly> SqrtEx<Poly>::operator+( const SqrtEx<Poly>& rhs ) const
    {
        assert( !has_sqrt() ||!rhs.has_sqrt() || radicand() == rhs.radicand() );
        SqrtEx<Poly> result = SqrtEx<Poly>( rhs.denominator() * constant_part() + rhs.constant_part() * denominator(),
                         rhs.denominator() * factor() + rhs.factor() * denominator(),
                         denominator() * rhs.denominator(), radicand() );
        return result;
    }

	template<typename Poly>
    SqrtEx<Poly> SqrtEx<Poly>::operator-( const SqrtEx<Poly>& rhs ) const
    {
        assert( !has_sqrt() || !rhs.has_sqrt() || radicand() == rhs.radicand() );
        SqrtEx<Poly> result = SqrtEx<Poly>( rhs.denominator() * constant_part() - rhs.constant_part() * denominator(),
                         rhs.denominator() * factor() - rhs.factor() * denominator(),
                         denominator() * rhs.denominator(), radicand() );
        return result;
    }

	template<typename Poly>
    SqrtEx<Poly> SqrtEx<Poly>::operator*( const SqrtEx<Poly>& rhs ) const
    {
        assert( !has_sqrt() || !rhs.has_sqrt() || radicand() == rhs.radicand() );
        SqrtEx<Poly> result = SqrtEx<Poly>( rhs.constant_part() * constant_part() + rhs.factor() * factor() * radicand(),
                         rhs.constant_part() * factor() + rhs.factor() * constant_part(),
                         denominator() * rhs.denominator(), radicand() );
        return result;
    }

	template<typename Poly>
    SqrtEx<Poly> SqrtEx<Poly>::operator/( const SqrtEx<Poly>& rhs ) const
    {
        assert( !rhs.has_sqrt() );
        SqrtEx<Poly> result = SqrtEx<Poly>( constant_part() * rhs.denominator(), factor() * rhs.denominator(),
                         denominator() * rhs.factor(), radicand() );
        return result;
    }
    
	template<typename Poly>
    inline std::ostream& operator<<( std::ostream& _out, const SqrtEx<Poly>& _substitution )
    {
        return (_out << _substitution.toString( true ) );
    }
    
	template<typename Poly>
    std::string SqrtEx<Poly>::toString( bool _infix, bool /*_friendlyNames*/ ) const
    {
        if( _infix )
        {
            bool complexNum = has_sqrt() && !m_constant_part.is_constant();
            std::stringstream result;
            if( complexNum && !carl::is_one(m_denominator) )
                result << "(";
            if( has_sqrt() )
            {
                if( m_constant_part.is_constant() )
                    result << m_constant_part;
                else
                    result << "(" << m_constant_part << ")";
                result << "+";
                if( m_factor.is_constant() )
                    result << m_factor;
                else
                    result << "(" << m_factor << ")";
                result << "*sqrt(" << m_radicand << ")";
            }
            else
            {
                if( m_constant_part.is_constant() || carl::is_one(m_denominator))
                    result << m_constant_part;
                else
                    result << "(" << m_constant_part << ")";
            }
            if (!carl::is_one(m_denominator))
            {
                if( complexNum )
                    result << ")";
                result << "/";
                if( m_denominator.is_constant() )
                    result << m_denominator;
                else
                    result << "(" << m_denominator << ")";
            }
            return result.str();
        }
        else
        {
            std::stringstream result;
			result << "(/ (+ ";
            result << m_constant_part;
            result << " (* ";
            result << m_factor;
            result << " ";
            result << "(sqrt ";
            result << m_radicand;
            result << "))) ";
            result << m_denominator;
            result << ")";
            return result.str();
        }
    }
}    // end namspace vs
