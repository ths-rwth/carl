/**
 * General class for floating point numbers with different formats. Extend to
 * other types if necessary.
 * 
 * @file FLOAT_T.h
 * @author  Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * @since   2013-10-14
 * @version 2014-01-30
 */

#pragma once

#include <string>
#include <iostream>
#include <assert.h>

#include "config.h"
#include <math.h>
#include "roundingConversion.h"
#include "typetraits.h"
#include "../util/SFINAE.h"
#include "../core/logging.h"
#ifdef USE_MPFR_FLOAT
#include <mpfr.h>
#endif
#include <cmath>

namespace carl
{
    typedef long precision_t;

    template<typename FloatType>
    class FLOAT_T;

    template<typename T1, typename T2>
    struct FloatConv
    {

        FLOAT_T<T1> operator() (const FLOAT_T<T2>& _op2) const
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
            mValue = FloatType(0);
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

        template<typename F = FloatType, DisableIf< std::is_same<F, double> > = dummy>
        FLOAT_T<FloatType>(const FloatType& val)
        {
            mValue = val;
        }

        ~FLOAT_T() { }

        /*******************
         * Getter & Setter *
         *******************/

        const FloatType& value() const
        {
            return mValue;
        }

        precision_t precision() const
        {
            return 0;
        }

        FLOAT_T<FloatType>& setPrecision(const precision_t&)
        {
            return *this;
        }

        /*************
         * Operators *
         *************/

        FLOAT_T<FloatType>& operator =(const FLOAT_T<FloatType> & _rhs)
        {
            mValue = _rhs.mValue;
            return *this;
        }

        /**
         * Boolean operators 
         */

        bool operator ==(const FLOAT_T<FloatType>& _rhs) const
        {
            return mValue == _rhs.mValue;
        }

        bool operator !=(const FLOAT_T<FloatType> & _rhs) const
        {
            return mValue != _rhs.mValue;
        }

        bool operator>(const FLOAT_T<FloatType> & _rhs) const
        {
            return mValue > _rhs.mValue;
        }

        bool operator<(const FLOAT_T<FloatType> & _rhs) const
        {
            return mValue < _rhs.mValue;
        }

        bool operator <=(const FLOAT_T<FloatType> & _rhs) const
        {
            return mValue <= _rhs.mValue;
        }

        bool operator >=(const FLOAT_T<FloatType> & _rhs) const
        {
            return mValue >= _rhs.mValue;
        }

        /**
         * arithmetic operations
         */

        FLOAT_T<FloatType>& add_assign(const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N)
        {
            mValue = mValue + _op2.mValue;
            return *this;
        }

        FLOAT_T<FloatType>& add(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = mValue + _op2.mValue;
            return _result;
        }

        FLOAT_T<FloatType>& sub_assign(const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N)
        {
            mValue = mValue - _op2.mValue;
            return *this;
        }

        FLOAT_T<FloatType>& sub(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = mValue - _op2.mValue;
            return _result;
        }

        FLOAT_T<FloatType>& mul_assign(const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N)
        {
            mValue = mValue * _op2.mValue;
            return *this;
        }

        FLOAT_T<FloatType>& mul(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = mValue * _op2.mValue;
            return _result;
        }

        FLOAT_T<FloatType>& div_assign(const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N)
        {
            assert(_op2 != 0);
            mValue = mValue / _op2.mValue;
            return *this;
        }

        FLOAT_T<FloatType>& div(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N) const
        {
            assert(_op2 != 0);
            _result.mValue = mValue / _op2.mValue;
            return _result;
        }

        /**
         * special operators
         */

        FLOAT_T<FloatType>& sqrt_assign(CARL_RND = CARL_RND::N)
        {
            assert(*this >= 0);
            mValue = std::sqrt(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& sqrt(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            assert(*this >= 0);
            _result.mValue = std::sqrt(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& cbrt_assign(CARL_RND = CARL_RND::N)
        {
            assert(*this >= 0);
            mValue = std::cbrt(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& cbrt(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            assert(*this >= 0);
            _result.mValue = std::cbrt(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& root_assign(unsigned long int, CARL_RND = CARL_RND::N)
        {
            assert(*this >= 0);
            /// @TODO implement root_assign for FLOAT_T
            LOG_NOTIMPLEMENTED();
            return *this;
        }

        FLOAT_T<FloatType>& root(FLOAT_T<FloatType>&, unsigned long int, CARL_RND = CARL_RND::N) const
        {
            assert(*this >= 0);
            LOG_NOTIMPLEMENTED();
            /// @TODO implement root for FLOAT_T
        }

        FLOAT_T<FloatType>& pow_assign(unsigned long int _exp, CARL_RND = CARL_RND::N)
        {
            mValue = std::pow(mValue, _exp);
            return *this;
        }

        FLOAT_T<FloatType>& pow(FLOAT_T<FloatType>& _result, unsigned long int _exp, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::pow(mValue, _exp);
            return _result;
        }

        FLOAT_T<FloatType>& abs_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::abs(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& abs(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::abs(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& exp_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::exp(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& exp(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::exp(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& sin_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::sin(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& sin(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::sin(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& cos_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::cos(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& cos(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::cos(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& log_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::log(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& log(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::log(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& tan_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::tan(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& tan(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::tan(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& asin_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::asin(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& asin(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::asin(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& acos_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::acos(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& acos(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::acos(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& atan_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::atan(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& atan(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::atan(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& sinh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::sinh(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& sinh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::sinh(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& cosh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::cosh(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& cosh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::cosh(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& tanh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::tanh(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& tanh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::tanh(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& asinh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::asinh(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& asinh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::asinh(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& acosh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::acosh(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& acosh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::acosh(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& atanh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::atanh(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& atanh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::atanh(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& floor(int& _result, CARL_RND = CARL_RND::N) const
        {
            _result = std::floor(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& floor_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::floor(mValue);
            return *this;
        }

        FLOAT_T<FloatType>& ceil(int& _result, CARL_RND = CARL_RND::N) const
        {
            _result = std::ceil(mValue);
            return _result;
        }

        FLOAT_T<FloatType>& ceil_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::ceil(mValue);
            return *this;
        }

        /**
         * conversion operators
         */
        double toDouble(CARL_RND = CARL_RND::N) const
        {
            return (double) mValue;
        }

        friend std::ostream & operator<<(std::ostream& ostr, const FLOAT_T<FloatType>& p)
        {
            ostr << p.toString();
            return ostr;
        }

        friend bool operator==(const FLOAT_T<FloatType>& _lhs, const int _rhs)
        {
            return _lhs.mValue == _rhs;
        }

        friend bool operator==(const int _lhs, const FLOAT_T<FloatType>& _rhs)
        {
            return _rhs == _lhs;
        }

        friend bool operator==(const FLOAT_T<FloatType>& _lhs, const double _rhs)
        {
            return _lhs.mValue == _rhs;
        }

        friend bool operator==(const double _lhs, const FLOAT_T<FloatType>& _rhs)
        {
            return _rhs == _lhs;
        }

        friend bool operator==(const FLOAT_T<FloatType>& _lhs, const float _rhs)
        {
            return _lhs.mValue == _rhs;
        }

        friend bool operator==(const float _lhs, const FLOAT_T<FloatType>& _rhs)
        {
            return _rhs == _lhs;
        }

        /**
         * operations required for the usage of Eigen3
         */

        inline const FLOAT_T<FloatType>& ei_conj(const FLOAT_T<FloatType>& x)
        {
            return x;
        }

        inline const FLOAT_T<FloatType>& ei_real(const FLOAT_T<FloatType>& x)
        {
            return x;
        }

        inline FLOAT_T<FloatType> ei_imag(const FLOAT_T<FloatType>&)
        {
            return FLOAT_T<FloatType>(0);
        }

        inline FLOAT_T<FloatType> ei_abs(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.abs(res);
            return res;
        }

        inline FLOAT_T<FloatType> ei_abs2(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.mul(res, x);
            return res;
        }

        inline FLOAT_T<FloatType> ei_sqrt(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.sqrt(res);
            return res;
        }

        inline FLOAT_T<FloatType> ei_exp(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.exp(res);
            return res;
        }

        inline FLOAT_T<FloatType> ei_log(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.log(res);
            return res;
        }

        inline FLOAT_T<FloatType> ei_sin(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.sin(res);
            return res;
        }

        inline FLOAT_T<FloatType> ei_cos(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.cos(res);
            return res;
        }

        inline FLOAT_T<FloatType> ei_pow(const FLOAT_T<FloatType>& x, FLOAT_T<FloatType> y)
        {
            FLOAT_T<FloatType> res;
            x.pow(res, unsigned(y));
            return res;
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
            _num.mValue += 1;
            return _num;
        }

        friend FLOAT_T<FloatType>& operator --(FLOAT_T<FloatType>& _num)
        {
            _num.mValue -= 1;
            return _num;
        }

        FLOAT_T<FloatType>& operator +=(const FLOAT_T<FloatType>& _rhs)
        {
            mValue = mValue + _rhs.mValue;
            return *this;
        }

        FLOAT_T<FloatType>& operator +=(const FloatType& _rhs)
        {
            mValue = mValue + _rhs;
            return *this;
        }

        FLOAT_T<FloatType>& operator -=(const FLOAT_T<FloatType>& _rhs)
        {
            mValue = mValue - _rhs.mValue;
            return *this;
        }

        FLOAT_T<FloatType>& operator -=(const FloatType& _rhs)
        {
            mValue = mValue - _rhs;
            return *this;
        }

        FLOAT_T<FloatType>& operator *=(const FLOAT_T<FloatType>& _rhs)
        {
            mValue = mValue * _rhs.mValue;
            return *this;
        }

        FLOAT_T<FloatType>& operator *=(const FloatType& _rhs)
        {
            mValue = mValue * _rhs;
            return *this;
        }

        FLOAT_T<FloatType>& operator /=(const FLOAT_T<FloatType>& _rhs)
        {
            mValue = mValue / _rhs.mValue;
            return *this;
        }

        FLOAT_T<FloatType>& operator /=(const FloatType& _rhs)
        {
            mValue = mValue / _rhs;
            return *this;
        }

        /**
         * Auxiliary Functions
         */

        std::string toString() const
        {
            return std::to_string(mValue);
        }
    };
#ifdef USE_MPFR_FLOAT
#include "floatTypes/mpfr_float.tpp"
#endif
}