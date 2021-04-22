/**
 * Class to create a square root expression object.
 * @author Florian Corzilius
 * @since 2011-05-26
 * @version 2013-10-22
 */

#pragma once

#include <carl/formula/Constraint.h>

#include <carl/core/Variable.h>
#include <carl/numbers/numbers.h>

#include <cassert>
#include <iostream>

namespace carl
{
	template<typename Poly>
    class SqrtEx {
		public:
			using Rational = typename UnderlyingNumberType<Poly>::type;
        private:
            /// The constant part c of this square root expression (c + f * sqrt(r))/d.
            Poly mConstantPart;
            /// The factor f of this square root expression (c + f * sqrt(r))/d.
            Poly mFactor;
            /// The denominator d of this square root expression (c + f * sqrt(r))/d.
            Poly mDenominator;
            /// The radicand r of this square root expression (c + f * sqrt(r))/d.
            Poly mRadicand;

        public:
            /**
             * Default Constructor. ( constructs (0 + 0 * sqrt( 0 )) / 1 )
             */
            SqrtEx();
            
            /**
             * Constructs a square root expression from a polynomial p leading to (p + 0 * sqrt( 0 )) / 1
             * @param _poly The polynomial to construct a square root expression for.
             */
            explicit SqrtEx( Poly&& _poly );
            explicit SqrtEx( const Poly& _poly ) : 
                SqrtEx::SqrtEx( Poly( _poly ) )
            {}
            
            explicit SqrtEx( Variable::Arg _var ) : 
                SqrtEx( Poly( _var ) )
            {}
            
            /**
             * Constructs a square root expression from given constant part, factor, denominator and radicand.
             * @param _constantPart The constant part of the square root expression to construct.
             * @param _factor The factor of the square root expression to construct.
             * @param _denominator The denominator of the square root expression to construct.
             * @param _radicand The radicand of the square root expression to construct.
             */
            SqrtEx( const Poly& _constantPart, const Poly& _factor, const Poly& _denominator, const Poly& _radicand ):
                SqrtEx( Poly( _constantPart ), Poly( _factor ), Poly( _denominator ), Poly( _radicand ) )
            {}
            
            SqrtEx( Poly&& _constantPart, Poly&& _factor, Poly&& _denominator, Poly&& _radicand );
            
            /**
             * @return A constant reference to the constant part of this square root expression.
             */
            const Poly& constantPart() const
            {
                return mConstantPart;
            }
            
            /**
             * @return A constant reference to the factor of this square root expression.
             */
            const Poly& factor() const
            {
                return mFactor;
            }

            /**
             * @return A constant reference to the denominator of this square root expression.
             */
            const Poly& denominator() const
            {
                return mDenominator;
            }

            /**
             * @return A constant reference to the radicand of this square root expression.
             */
            const Poly& radicand() const
            {
                return mRadicand;
            }

            /**
             * @return true, if the square root expression has a non trivial radicand;
             *          false, otherwise.
             */
            bool hasSqrt() const
            {
                return !carl::isZero(mFactor);
            }

            /**
             * @return true, if the square root expression can be expressed as a polynomial;
             *          false, otherwise.
             */
            bool isPolynomial() const
            {
                return carl::isZero(mFactor) && mDenominator.isConstant();
            }

            /**
             * @return The square root expression as a polynomial (note that there must be no square root nor denominator
             */
            Poly asPolynomial() const
            {
                assert( isPolynomial() );
                assert( !carl::isZero(mDenominator) );
                return mConstantPart / mDenominator.constantPart();
            }

            /**
             * @return true, if there is no variable in this square root expression;
             *          false, otherwise.
             */
            bool isConstant() const
            {
                return mConstantPart.isConstant() && mDenominator.isConstant() && mFactor.isConstant() && mRadicand.isConstant();
            }
            
            /**
             * @return This sqrtEx as an integer (note, that it must actually represent an integer then).
             */
            Rational asConstant() const
            {
                assert( isConstant() );
                return mConstantPart.constantPart();
            }

            /**
             * @return true, if there is no variable in this square root expression;
             *          false, otherwise.
             */
            bool isRational() const
            {
                return mConstantPart.isConstant() && mDenominator.isConstant() && carl::isZero(mRadicand);
            }
            
            /**
             * @return This sqrtEx as a rational (note, that it must actually represent a rational then).
             */
            Rational asRational() const
            {
                if( isConstant() )
                    return asConstant();
                assert( isRational() );
                return mConstantPart.constantPart()/mFactor.constantPart();
            }
            
        private:
            
            /**
             * Normalizes this object, that is extracts as much as possible from the radicand into the factor
             * and cancels the enumerator and denominator afterwards.
             */
            void normalize();
            
        public:
            
            /**
             * @return true, if the this square root expression corresponds to an integer value;
             *         false, otherwise.
             */
            bool isInteger() const
            {
                return carl::isZero(radicand()) && carl::isOne(denominator()) && 
                       (carl::isZero(constantPart()) || (constantPart().isConstant() && carl::isInteger( constantPart().lcoeff() ) ) );
            }
            
            /**
             * @param _sqrtEx Square root expression to compare with.
             * @return  true, if this square root expression and the given one are equal;
             *          false, otherwise.
             */
            bool operator==( const SqrtEx& _toCompareWith ) const;
            
            /**
             * @param _sqrtEx A square root expression, which gets the new content of this square root expression.
             * @return A reference to this object.
             */
            SqrtEx& operator=( const SqrtEx& _sqrtEx );
            
            /**
             * @param _poly A polynomial, which gets the new content of this square root expression.
             * @return A reference to this object.
             */
            SqrtEx& operator=( const Poly& _poly );
            
            /**
             * @param _summandA  First summand.
             * @param _summandB  Second summand.
             * @return The sum of the given square root expressions.
             */
            SqrtEx operator+( const SqrtEx& rhs ) const;
            
            /**
             * @param _minuend  Minuend.
             * @param _subtrahend  Subtrahend.
             * @return The difference of the given square root expressions.
             */
            SqrtEx operator-( const SqrtEx& rhs ) const;
      
            /**
             * @param _factorA  First factor.
             * @param _factorB  Second factor.
             * @return The product of the given square root expressions.
             */
            SqrtEx operator*( const SqrtEx& rhs ) const;
            
            /**
             * @param _dividend  Dividend.
             * @param _divisor  Divisor.
             * @return The result of the first given square root expression divided by the second one
             *          Note that the second argument is not allowed to contain a square root.
             */
            SqrtEx operator/( const SqrtEx& rhs ) const;
            
            /**
             * Prints the given square root expression on the given stream.
             * @param _out The stream to print on.
             * @param _sqrtEx The square root expression to print.
             * @return The stream after printing the square root expression on it.
             */
			template<typename P>
            friend std::ostream& operator<<( std::ostream& _out, const SqrtEx<P>& _sqrtEx );
            
            /**
             * @param _infix A string which is printed in the beginning of each row.
             * @param _friendlyNames A flag that indicates whether to print the variables with their internal representation (false)
             *                        or with their dedicated names.
             * @return The string representation of this square root expression.
             */
            std::string toString( bool _infix = false, bool _friendlyNames = true ) const;
            
            bool evaluate( Rational& _result, const std::map<Variable, Rational>& _evalMap, int _rounding = 0 ) const;
            
            SqrtEx substitute( const std::map<Variable, Rational>& _evalMap ) const;
            
            /**
             * Substitutes a variable in an expression by a square root expression, which results in a square root expression.
             * @param _substituteIn The polynomial to substitute in.
             * @param _varToSubstitute The variable to substitute.
             * @param _substituteBy The square root expression by which the variable gets substituted.
             * @return The resulting square root expression.
             */
            static SqrtEx subBySqrtEx( const Poly& _substituteIn, const carl::Variable& _varToSubstitute, const SqrtEx& _substituteBy );
    };

    template<typename Poly>
    void variables(const SqrtEx<Poly>& ex, carlVariables& vars) {
        variables(ex.constantPart(), vars);
        variables(ex.factor(), vars);
        variables(ex.denominator(), vars);
        variables(ex.radicand(), vars);
	}

}    // end namspace vs

namespace std
{
    /**
     * Implements std::hash for square root expressions.
     */
    template<typename Poly>
    struct hash<carl::SqrtEx<Poly>>
    {
    public:
        /**
         * @param _sqrtEx The square root expression to get the hash for.
         * @return The hash of the given square root expression.
         */
        std::size_t operator()( const carl::SqrtEx<Poly>& _sqrtEx ) const 
        {
            return ((hash<Poly>()(_sqrtEx.radicand()) ^ hash<Poly>()(_sqrtEx.denominator())) ^ hash<Poly>()(_sqrtEx.factor())) ^ hash<Poly>()(_sqrtEx.constantPart());
        }
    };
} // namespace std

#include "SqrtEx.tpp"
