/**
 * Class to create a square root expression object.
 * @author Florian Corzilius
 * @since 2011-05-26
 * @version 2013-10-07
 */

#include <carl/core/polynomialfunctions/GCD.h>

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
        m_factor( isZero(_radicand) ? std::move( _radicand ) : std::move( _factor ) ),
        m_denominator( (isZero(m_factor) && isZero(m_constant_part)) ? constant_one<Poly>::get() : std::move( _denominator ) ),
        m_radicand( isZero(m_factor) ? m_factor : std::move( _radicand ) )
    {
        assert( !isZero(m_denominator) );
        assert( !m_radicand.isConstant() || isZero(m_radicand) || constant_zero<Rational>::get() <= m_radicand.trailingTerm().coeff() );
        normalize();
    }

	template<typename Poly>
    void SqrtEx<Poly>::normalize()
    {
//        std::cout << *this << std::endl;
        Poly gcdA;
        if( isZero(m_factor) )
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
                assert( !isZero(radicand()) );
                Rational absOfLCoeff = abs( radicand().coprimeFactor() );
                Rational sqrtResult = 0;
                if( carl::sqrt_exact( absOfLCoeff, sqrtResult ) )
                {
                    m_factor *= constant_one<Rational>::get()/sqrtResult;
                    m_radicand *= absOfLCoeff;
                }
            }
            if( isZero(m_factor) )
            {
                gcdA = m_constant_part;
            }
            else
            {
                if( isZero(m_constant_part) )
                {
                    gcdA = m_factor;
                }
                else
                {
                    Rational ccConstantPart = m_constant_part.coprimeFactor();
                    Poly cpConstantPart = m_constant_part * ccConstantPart;
                    Rational ccFactor = m_factor.coprimeFactor();
                    Poly cpFactor = m_factor * ccFactor;
                    gcdA = carl::gcd( cpConstantPart, cpFactor )*carl::gcd(ccConstantPart,ccFactor);
                }
            }
        }
        if( isZero(gcdA) ) return;
        Rational ccGcdA = gcdA.coprimeFactor();
        Poly cpGcdA = gcdA * ccGcdA;
        Rational ccDenominator = m_denominator.coprimeFactor();
        Poly cpDenominator = m_denominator * ccDenominator;
        gcdA = carl::gcd( cpGcdA, cpDenominator )*carl::gcd(ccGcdA,ccDenominator);
        // Make sure that the polynomial to divide by cannot be negative, otherwise the sign of the square root expression could change.
        if( !(gcdA == constant_one<Poly>::get()) && carl::definiteness(gcdA) == carl::Definiteness::POSITIVE_SEMI )
        {
            if( !isZero(m_constant_part) )
            {
				carl::try_divide(m_constant_part, gcdA, m_constant_part );
            }
            if( !isZero(m_factor) )
            {
                carl::try_divide(m_factor, gcdA, m_factor );
            }
            carl::try_divide(m_denominator, gcdA, m_denominator );
        }
        Rational numGcd = constant_zero<Rational>::get();
        Rational denomLcm = constant_one<Rational>::get();
        if( isZero(factor()) )
        {
            if( !isZero(constantPart()) )
            {
                Rational cpOfConstantPart = constantPart().coprimeFactor();
                numGcd = carl::getNum( cpOfConstantPart );
                denomLcm = carl::getDenom( cpOfConstantPart );
            }
            else
            {
//                std::cout << "        to " << *this << std::endl;
                return; // Sqrt expression corresponds to 0.
            }
        }
        else
        {
            Rational cpOfFactorPart = factor().coprimeFactor();
            if( isZero(constantPart()) )
            {
                numGcd = carl::getNum( cpOfFactorPart );
                denomLcm = carl::getDenom( cpOfFactorPart );
            }
            else
            {
                Rational cpOfConstantPart = constantPart().coprimeFactor();
                numGcd = carl::gcd( carl::getNum( cpOfConstantPart ), carl::getNum( cpOfFactorPart ) );
                denomLcm = carl::lcm( carl::getDenom( cpOfConstantPart ), carl::getDenom( cpOfFactorPart ) );
            }
        }
        assert( numGcd != constant_zero<Rational>::get() );
        Rational cpFactor = numGcd/denomLcm; 
        m_constant_part *= cpFactor;
        m_factor *= cpFactor;
        Rational cpOfDenominator = denominator().coprimeFactor();
        m_denominator *= cpOfDenominator;
        Rational sqrtExFactor = (denomLcm*carl::getNum( cpOfDenominator ))/(numGcd*carl::getDenom( cpOfDenominator ));
        m_constant_part *= carl::getNum( sqrtExFactor );
        m_factor *= carl::getNum( sqrtExFactor );
        m_denominator *= carl::getDenom( sqrtExFactor );
//        std::cout << "       to  " << *this << std::endl;
        //TODO: implement this method further
    }

	template<typename Poly>
    bool SqrtEx<Poly>::operator==( const SqrtEx& _toCompareWith ) const
    {
        return    m_radicand == _toCompareWith.radicand() && m_denominator == _toCompareWith.denominator() 
               && m_factor == _toCompareWith.factor() && m_constant_part == _toCompareWith.constantPart();
    }

	template<typename Poly>
    SqrtEx<Poly>& SqrtEx<Poly>::operator=( const SqrtEx<Poly>& _sqrtEx )
    {
        if( this != &_sqrtEx )
        {
            m_constant_part = _sqrtEx.constantPart();
            m_factor       = _sqrtEx.factor();
            m_denominator  = _sqrtEx.denominator();
            if (isZero(factor()))
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
        SqrtEx<Poly> result = SqrtEx<Poly>( rhs.denominator() * constantPart() + rhs.constantPart() * denominator(),
                         rhs.denominator() * factor() + rhs.factor() * denominator(),
                         denominator() * rhs.denominator(), radicand() );
        return result;
    }

	template<typename Poly>
    SqrtEx<Poly> SqrtEx<Poly>::operator-( const SqrtEx<Poly>& rhs ) const
    {
        assert( !has_sqrt() || !rhs.has_sqrt() || radicand() == rhs.radicand() );
        SqrtEx<Poly> result = SqrtEx<Poly>( rhs.denominator() * constantPart() - rhs.constantPart() * denominator(),
                         rhs.denominator() * factor() - rhs.factor() * denominator(),
                         denominator() * rhs.denominator(), radicand() );
        return result;
    }

	template<typename Poly>
    SqrtEx<Poly> SqrtEx<Poly>::operator*( const SqrtEx<Poly>& rhs ) const
    {
        assert( !has_sqrt() || !rhs.has_sqrt() || radicand() == rhs.radicand() );
        SqrtEx<Poly> result = SqrtEx<Poly>( rhs.constantPart() * constantPart() + rhs.factor() * factor() * radicand(),
                         rhs.constantPart() * factor() + rhs.factor() * constantPart(),
                         denominator() * rhs.denominator(), radicand() );
        return result;
    }

	template<typename Poly>
    SqrtEx<Poly> SqrtEx<Poly>::operator/( const SqrtEx<Poly>& rhs ) const
    {
        assert( !rhs.has_sqrt() );
        SqrtEx<Poly> result = SqrtEx<Poly>( constantPart() * rhs.denominator(), factor() * rhs.denominator(),
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
            bool complexNum = has_sqrt() && !m_constant_part.isConstant();
            std::stringstream result;
            if( complexNum && !carl::isOne(m_denominator) )
                result << "(";
            if( has_sqrt() )
            {
                if( m_constant_part.isConstant() )
                    result << m_constant_part;
                else
                    result << "(" << m_constant_part << ")";
                result << "+";
                if( m_factor.isConstant() )
                    result << m_factor;
                else
                    result << "(" << m_factor << ")";
                result << "*sqrt(" << m_radicand << ")";
            }
            else
            {
                if( m_constant_part.isConstant() || carl::isOne(m_denominator))
                    result << m_constant_part;
                else
                    result << "(" << m_constant_part << ")";
            }
            if (!carl::isOne(m_denominator))
            {
                if( complexNum )
                    result << ")";
                result << "/";
                if( m_denominator.isConstant() )
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
