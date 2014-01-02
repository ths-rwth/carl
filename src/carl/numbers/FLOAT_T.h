/**
 * General class for floating point numbers with different formats. Extend to
 * other types if necessary.
 * 
 * @file FLOAT_T.h
 * @author  Stefan Schupp
 * @since   2013-10-14
 * @version 2013-10-14
 */

#pragma once

#include <string>
#include <iostream>
#include <assert.h>

#include "config.h"
#include <math.h>
#include "roundingConversion.h"
#include "typetraits.h"
#include <mpfr.h>
#include <cmath>

namespace carl
{
		// nearest, towards zero, towards infty, towards -infty, away from zero
//    enum CARL_RND
//    {
//        CARL_RND::N=0,
//        CARL_RNDZ=1,
//        CARL_RNDU=2,
//        CARL_RNDD=3,
//        CARL_RNDA=4
//    };

    typedef unsigned long precision;
    
    template<typename FloatType>
    class FLOAT_T;
    
    template<typename T1, typename T2>
    struct FloatConv
    {
        FLOAT_T<T1> operator() ( const FLOAT_T<T2>& _op2 ) const
        {
            return FLOAT_T<T1>(_op2.toDouble());
        }
    };
    
    template<typename FloatType>
    class FLOAT_T
    {
        private:
            FloatType mValue;
        
        public:
    	
            /**
             * Constructors & Destructors
             */

            FLOAT_T<FloatType>()
            {
		assert(is_float<FloatType>::value);
                mValue = 0;
            }
        
            FLOAT_T<FloatType>(const double _double)
            {
		assert(is_float<FloatType>::value);
                mValue = _double;
            }
            
            FLOAT_T<FloatType>(const float _float)
            {
		assert(is_float<FloatType>::value);
                mValue = _float;
            }
            
            FLOAT_T<FloatType>(const int _int)
            {
                assert(is_float<FloatType>::value);
                mValue = _int;
            }
            
            FLOAT_T<FloatType>(const FLOAT_T<FloatType>& _float) : mValue(_float.mValue)
            {
                assert(is_float<FloatType>::value);
            }
            
            ~FLOAT_T()
            {}

            
            /*******************
            * Getter & Setter *
            *******************/

           const FloatType& getValue() const
           {
               return mValue;
           }

           precision getPrec() const
           {
               // TODO
               return 0;
           }

           FLOAT_T<FloatType>& setPrec( const precision& _prec)
           {
               // TODO
               return *this;
           }
            
            /*************
             * Operators *
             *************/
            
            FLOAT_T<FloatType>& operator = (const FLOAT_T<FloatType> & _rhs)
            {
                mValue = _rhs.mValue;
                return *this;
            }
            
            /**
             * Boolean operators 
             */
            
            bool operator == ( const FLOAT_T<FloatType>& _rhs) const
            {
                return mValue == _rhs.mValue;
            }

            bool operator != ( const FLOAT_T<FloatType> & _rhs) const
            {
                return mValue != _rhs.mValue;
            }

            bool operator > ( const FLOAT_T<FloatType> & _rhs) const
            {
                return mValue > _rhs.mValue;
            }

            bool operator < ( const FLOAT_T<FloatType> & _rhs) const
            {
                return mValue < _rhs.mValue;
            }

            bool operator <= ( const FLOAT_T<FloatType> & _rhs) const
            {
                return mValue <= _rhs.mValue;
            }

            bool operator >= ( const FLOAT_T<FloatType> & _rhs) const
            {
                return mValue >= _rhs.mValue;
            }
            
            /**
             * arithmetic operations
             */
            
            FLOAT_T<FloatType>& add_assign( const FLOAT_T<FloatType>& _op2, CARL_RND _rnd = CARL_RND::N)
            {
                // TODO: Include rounding
                mValue = mValue + _op2.mValue;
                return *this;
            }
            
            void add( FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND _rnd = CARL_RND::N) const
            {
                // TODO: Include rounding
                _result.mValue = mValue + _op2.mValue;
            }

            FLOAT_T<FloatType>& sub_assign(const FLOAT_T<FloatType>& _op2, CARL_RND _rnd = CARL_RND::N)
            {
                // TODO: Include rounding
                mValue = mValue - _op2.mValue;
                return *this;
            }
            
            void sub(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND _rnd = CARL_RND::N) const
            {
                // TODO: Include rounding
                _result.mValue = mValue - _op2.mValue;
            }
            
            FLOAT_T<FloatType>& mul_assign(const FLOAT_T<FloatType>& _op2, CARL_RND _rnd = CARL_RND::N)
            {
                // TODO: Include rounding
                mValue = mValue * _op2.mValue;
                // mValue.mul_assign(_op2.mValue, _rnd);
                return *this;
            }
            
            void mul(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND _rnd = CARL_RND::N) const
            {
                // TODO: Include rounding
                _result.mValue = mValue * _op2.mValue;
            }
            
            FLOAT_T<FloatType>& div_assign(const FLOAT_T<FloatType>& _op2, CARL_RND _rnd = CARL_RND::N)
            {
                assert( _op2 != 0 );
                // TODO: Include rounding
                mValue = mValue / _op2.mValue;
                return *this;
            }
            
            void div(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND _rnd = CARL_RND::N) const
            {
                assert( _op2 != 0 );
                // TODO: Include rounding
                _result.mValue = mValue / _op2.mValue;
            }
            
            /**
             * special operators
             */

            FLOAT_T<FloatType>& sqrt_assign(CARL_RND _rnd = CARL_RND::N)
            {
                assert( *this >= 0);
                mValue = std::sqrt(mValue);
                return *this;
            }
            
            void sqrt(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                assert( *this >= 0);
            	_result.mValue = std::sqrt(mValue);
            }

            FLOAT_T<FloatType>& cbrt_assign(CARL_RND _rnd = CARL_RND::N)
            {
                assert( *this >= 0);
                mValue = std::cbrt(mValue);
                return *this;
            }
            
            FLOAT_T<FloatType>& cbrt(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                assert( *this >= 0);
            	_result.mValue = std::cbrt(mValue);
            }

            FLOAT_T<FloatType>& root_assign(unsigned long int _k, CARL_RND _rnd = CARL_RND::N)
            {
                assert( *this >= 0);
                // TODO
                return *this;
            }
            
            void root(FLOAT_T<FloatType>& _result, unsigned long int _k, CARL_RND _rnd = CARL_RND::N)
            {
                assert( *this >= 0);
            	// TODO
            }

            FLOAT_T<FloatType>& pow_assign(unsigned long int _exp, CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::pow(mValue, _exp);
                return *this;
            }
            
            void pow(FLOAT_T<FloatType>& _result, unsigned long int _exp, CARL_RND _rnd = CARL_RND::N)
            {
            	_result.mValue = std::pow(mValue, _exp);
            }

            FLOAT_T<FloatType>& abs_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::abs(mValue);
                return *this;
            }
            
            void abs(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
            	_result.mValue = std::abs(mValue);
            }
            
            FLOAT_T<FloatType>& exp_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::exp(mValue);
                return *this;
            }
            
            void exp(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::exp(mValue);
            }
            
            FLOAT_T<FloatType>& sin_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::sin(mValue);
                return *this;
            }
            
            void sin(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::sin(mValue);
            }
            
            FLOAT_T<FloatType>& cos_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::cos(mValue);
                return *this;
            }
            
            void cos(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::cos(mValue);
            }
            
            FLOAT_T<FloatType>& log_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::log(mValue);
                return *this;
            }
            
            void log(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::log(mValue);
            }
            
            FLOAT_T<FloatType>& tan_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::tan(mValue);
                return *this;
            }
            
            void tan(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::tan(mValue);
            }
            
            FLOAT_T<FloatType>& asin_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::asin(mValue);
                return *this;
            }
            
            void asin(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::asin(mValue);
            }
            
            FLOAT_T<FloatType>& acos_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::acos(mValue);
                return *this;
            }
            
            void acos(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::acos(mValue);
            }
            
            FLOAT_T<FloatType>& atan_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::atan(mValue);
                return *this;
            }
            
            void atan(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::atan(mValue);
            }

            FLOAT_T<FloatType>& sinh_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::sinh(mValue);
                return *this;
            }
            
            void sinh(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::sinh(mValue);
            }
            
            FLOAT_T<FloatType>& cosh_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::cosh(mValue);
                return *this;
            }
            
            void cosh(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::cosh(mValue);
            }
            
            FLOAT_T<FloatType>& tanh_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::tanh(mValue);
                return *this;
            }
            
            void tanh(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::tanh(mValue);
            }
            
            FLOAT_T<FloatType>& asinh_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::asinh(mValue);
                return *this;
            }
            
            void asinh(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::asinh(mValue);
            }
            
            FLOAT_T<FloatType>& acosh_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::acosh(mValue);
                return *this;
            }
            
            void acosh(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::acosh(mValue);
            }
            
            FLOAT_T<FloatType>& atanh_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::atanh(mValue);
                return *this;
            }
            
            void atanh(FLOAT_T<FloatType>& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result.mValue = std::atanh(mValue);
            }
            
            void floor(int& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result = std::floor(mValue);
            }
            
            FLOAT_T<FloatType>& floor_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::floor(mValue);
                return *this;
            }
            
            void ceil(int& _result, CARL_RND _rnd = CARL_RND::N)
            {
                _result = std::ceil(mValue);
            }
            
            FLOAT_T<FloatType>& ceil_assign(CARL_RND _rnd = CARL_RND::N)
            {
                mValue = std::ceil(mValue);
                return *this;
            }
            
            /**
             * conversion operators
             */
            double toDouble(CARL_RND _rnd=CARL_RND::N) const
            {
                return (double) mValue;
            }
            

            friend std::ostream & operator<< (std::ostream& ostr, const FLOAT_T<FloatType>& p) {
                ostr << p.toString();
                return ostr;
            }
            
            friend bool operator== (const FLOAT_T<FloatType>& _lhs, const int _rhs)
            {
                return _lhs.mValue == _rhs;
            }
            
            friend bool operator== (const int _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return _rhs == _lhs;
            }
            
            friend bool operator== (const FLOAT_T<FloatType>& _lhs, const double _rhs)
            {
                return _lhs.mValue == _rhs;
            }
            
            friend bool operator== (const double _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return _rhs == _lhs;
            }
            
            friend bool operator== (const FLOAT_T<FloatType>& _lhs, const float _rhs)
            {
                return _lhs.mValue == _rhs;
            }
            
            friend bool operator== (const float _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return _rhs == _lhs;
            }
            
            /**
             * Operators
             */
            
            friend FLOAT_T<FloatType> operator +(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return FLOAT_T<FloatType>(_lhs.mValue + _rhs.mValue);
            }
		
			friend FLOAT_T<FloatType> operator +(const FLOAT_T<FloatType>& _lhs, const FloatType& _rhs)
			{
				return FLOAT_T<FloatType>(_lhs.mValue + _rhs);
			}
        
			friend FLOAT_T<FloatType> operator +(const FloatType& _lhs, const FLOAT_T<FloatType>& _rhs)
			{
				return _rhs + _lhs;
			}
		
            friend FLOAT_T<FloatType> operator -(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return FLOAT_T<FloatType>(_lhs.mValue - _rhs.mValue);
            }

			friend FLOAT_T<FloatType> operator -(const FLOAT_T<FloatType>& _lhs, const FloatType& _rhs)
			{
				return FLOAT_T<FloatType>(_lhs.mValue - _rhs);
			}
			
			friend FLOAT_T<FloatType> operator -(const FloatType& _lhs, const FLOAT_T<FloatType>& _rhs)
			{
				return FLOAT_T<FloatType>(_lhs - _rhs.mValue);
			}
		
            friend FLOAT_T<FloatType> operator *(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return FLOAT_T<FloatType>(_lhs.mValue * _rhs.mValue);
            }
			
			friend FLOAT_T<FloatType> operator *(const FloatType& _lhs, const FLOAT_T<FloatType>& _rhs)
			{
				return FLOAT_T<FloatType>(_lhs * _rhs.mValue);
			}
			
			friend FLOAT_T<FloatType> operator *(const FLOAT_T<FloatType>& _lhs, const FloatType& _rhs)
			{
				return FLOAT_T<FloatType>(_lhs.mValue * _rhs);
			}
        
            friend FLOAT_T<FloatType> operator /(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return FLOAT_T<FloatType>(_lhs.mValue / _rhs.mValue);
            }

			friend FLOAT_T<FloatType> operator /(const FLOAT_T<FloatType>& _lhs, const FloatType& _rhs)
			{
				return FLOAT_T<FloatType>(_lhs.mValue / _rhs);
			}
			
			friend FLOAT_T<FloatType> operator /(const FloatType& _lhs, const FLOAT_T<FloatType>& _rhs)
			{
				return FLOAT_T<FloatType>(_lhs / _rhs.mValue);
			}
            friend FLOAT_T<FloatType>& operator ++(FLOAT_T<FloatType>& _num)
            {
                _num.mValue +=1;
                return _num;
            }
            
            friend FLOAT_T<FloatType>& operator --(FLOAT_T<FloatType>& _num)
            {
                _num.mValue -=1;
                return _num;
            }
            
            FLOAT_T<FloatType>& operator +=(const FLOAT_T<FloatType>& _rhs)
            {
                mValue + _rhs.mValue;
                return *this;
            }
        
			FLOAT_T<FloatType>& operator +=(const FloatType& _rhs)
			{
				mValue + _rhs;
				return *this;
			}
		
            FLOAT_T<FloatType>& operator -=(const FLOAT_T<FloatType>& _rhs)
            {
                mValue - _rhs.mValue;
                return *this;
            }
        
			FLOAT_T<FloatType>& operator -=(const FloatType& _rhs)
			{
				mValue - _rhs;
				return *this;
			}
		
            FLOAT_T<FloatType>& operator *=(const FLOAT_T<FloatType>& _rhs)
            {
                mValue * _rhs.mValue;
                return *this;
            }
        
			FLOAT_T<FloatType>& operator *=(const FloatType& _rhs)
			{
				mValue * _rhs;
				return *this;
			}
		
            FLOAT_T<FloatType>& operator /=(const FLOAT_T<FloatType>& _rhs)
            {
                mValue / _rhs.mValue;
                return *this;
            }
        
			FLOAT_T<FloatType>& operator /=(const FloatType& _rhs)
			{
				mValue / _rhs;
				return *this;
			}
		
            /**
             * Auxiliary Functions
             */
            
            std::string toString() const
            {
//                std::stringstream str;
//                str << mValue;   
//                return str.str();
                return std::to_string(mValue);
            }
    };
#ifdef USE_MPFR_FLOAT
    #include "floatTypes/mpfr_float.tpp"
#endif

}
