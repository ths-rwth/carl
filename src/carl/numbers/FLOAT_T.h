/**
 * General class for floating point numbers with different formats. Extend to
 * other types if necessary.
 * 
 * @file FLOAT_T.h
 * @author  Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * @since   2013-10-14
 * @version 2014-08-28
 */

#pragma once

#include <string>
#include <iostream>
#include <assert.h>
#include <math.h>
#include <cmath>
#include <cln/cln.h>
#ifdef USE_MPFR_FLOAT
#include <mpfr.h>
#endif

#include "typetraits.h"
#include "adaption_cln/typetraits.h"
#include "adaption_cln/operations.h"
#include "config.h"
#include "roundingConversion.h"
#include "../util/SFINAE.h"
#include "../core/logging.h"


namespace carl
{
    typedef long precision_t;

    template<typename FloatType>
    class FLOAT_T;

    /**
     * Struct which holds the conversion operator for any two instanciations of 
     * FLOAT_T with different underlying floating point implementations. Note
     * that this conversion introduces loss of precision, as it uses the toDouble()
     * method and the corresponding double constructor from the target type.
     */
    template<typename T1, typename T2>
    struct FloatConv
    {
        /**
         * Conversion operator for conversion of two instanciations of FLOAT_T 
         * with different underlying floating point implementations.
         * @param _op2 The source instanciation (T2)
         * @return returns an instanciation with different floating point implementation (T1)
         */
        FLOAT_T<T1> operator() (const FLOAT_T<T2>& _op2) const
        {
            return FLOAT_T<T1>(_op2.toDouble());
        }
    };
    
    /**
     * Templated wrapper class which allows universal usage of different 
     * IEEE 754 implementations.
     * For each implementation intended to use it is necessary to implement the
     * according specialization of this class.
     */
    template<typename FloatType>
    class FLOAT_T
    {
    private:
        FloatType mValue;

    public:

        /**
         * Constructors & Destructors
         */

        /**
         * Default empty constructor, which initializes to zero.
         */
        FLOAT_T<FloatType>()
        {
            assert(is_float<FloatType>::value);
            mValue = FloatType(0);
        }

        /**
         * Constructor, which takes a double as input and optional rounding, which
         * can be used, if the underlying fp implementation allows this.
         * @param _double Value to be initialized.
         * @param N
         */
        FLOAT_T<FloatType>(const double _double, const CARL_RND=CARL_RND::N)
        {
            assert(is_float<FloatType>::value);
            mValue = _double;
        }

        /**
         * Constructor, which takes a float as input and optional rounding, which
         * can be used, if the underlying fp implementation allows this.
         * @param _float Value to be initialized.
         * @param N
         */
        FLOAT_T<FloatType>(const float _float, const CARL_RND=CARL_RND::N)
        {
            assert(is_float<FloatType>::value);
            mValue = _float;
        }

        /**
         * Constructor, which takes an integer as input and optional rounding, which
         * can be used, if the underlying fp implementation allows this.
         * @param _int Value to be initialized.
         * @param N
         */
        FLOAT_T<FloatType>(const int _int, const CARL_RND=CARL_RND::N)
        {
            assert(is_float<FloatType>::value);
            mValue = _int;
        }
        
        /**
         * Constructor, which takes a long as input and optional rounding, which
         * can be used, if the underlying fp implementation allows this.
         * @param _long Value to be initialized.
         * @param N
         */
        FLOAT_T<FloatType>(const long _long, const CARL_RND=CARL_RND::N)
        {
            assert(is_float<FloatType>::value);
            mValue = _long;
        }

        /**
         * Copyconstructor which takes a FLOAT_T<FloatType>  and optional rounding 
         * as input, which can be used, if the underlying fp implementation 
         * allows this.
         * @param _float Value to be initialized.
         * @param N
         */
        FLOAT_T<FloatType>(const FLOAT_T<FloatType>& _float, const CARL_RND=CARL_RND::N) : mValue(_float.mValue)
        {
            assert(is_float<FloatType>::value);
        }

        /**
         * Constructor, which takes an arbitrary fp type as input and optional rounding, which
         * can be used, if the underlying fp implementation allows this.
         * @param val Value to be initialized.
         * @param N
         */
        template<typename F = FloatType, DisableIf< std::is_same<F, double> > = dummy>
        FLOAT_T<FloatType>(const FloatType& val, const CARL_RND=CARL_RND::N)
        {
            mValue = val;
        }
        
        /**
         * Constructor, which takes a FLOAT_T instanciation with different fp implementation
         * as input and optional rounding, which can be used, if the underlying 
         * fp implementation allows this.
         * @param _float Value to be initialized.
         * @param N
         */
        template<typename F, DisableIf< std::is_same<F, FloatType> > = dummy>
        FLOAT_T<FloatType>(const FLOAT_T<F>& _float, const CARL_RND=CARL_RND::N)
        {
            mValue = _float.toDouble();
        }

        /**
         * Destructor. Note that for some specializations memory management has to
         * be included here.
         */
        ~FLOAT_T() { }

        /*******************
         * Getter & Setter *
         *******************/

        /**
         * Getter for the raw value contained.
         * @return raw value.
         */
        const FloatType& value() const
        {
            return mValue;
        }

        /**
         * If precision is used, this getter returns the acutal precision (default:
         * 53 bit).
         * @return precision.
         */
        precision_t precision() const
        {
            return 0;
        }

        /**
         * Allows to set the desired precision. Note: If the value is already 
         * initialized this can change the internal value.
         * @param Precision in bits.
         * @return reference to this.
         */
        FLOAT_T<FloatType>& setPrecision(const precision_t&)
        {
            return *this;
        }

        /*************
         * Operators *
         *************/

        /**
         * Assignment operator.
         * @param _rhs Righthand side of the assignment.
         * @return 
         */
        FLOAT_T<FloatType>& operator =(const FLOAT_T<FloatType>& _rhs)
        {
            mValue = _rhs.mValue;
            return *this;
        }

        /**
         * Boolean operators 
         */

        /**
         * Comparison operator for equality.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        bool operator ==(const FLOAT_T<FloatType>& _rhs) const
        {
            return mValue == _rhs.mValue;
        }

        /**
         * Comparison operator for inequality.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        bool operator !=(const FLOAT_T<FloatType> & _rhs) const
        {
            return mValue != _rhs.mValue;
        }

        /**
         * Comparison operator for larger than.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        bool operator>(const FLOAT_T<FloatType> & _rhs) const
        {
            return mValue > _rhs.mValue;
        }

        /**
         * Comparison operator for less than.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        bool operator<(const FLOAT_T<FloatType> & _rhs) const
        {
            return mValue < _rhs.mValue;
        }

        /**
         * Comparison operator for less or equal than.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        bool operator <=(const FLOAT_T<FloatType> & _rhs) const
        {
            return mValue <= _rhs.mValue;
        }

        /**
         * Comparison operator for larger or equal than.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        bool operator >=(const FLOAT_T<FloatType> & _rhs) const
        {
            return mValue >= _rhs.mValue;
        }

        /**
         * arithmetic operations
         */

        /**
         * Function for addition of two numbers, which assigns the result to the 
         * calling number.
         * @param _op2 Righthand side of the operation
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& add_assign(const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N)
        {
            mValue = mValue + _op2.mValue;
            return *this;
        }

        /**
         * Function which adds two numbers and puts the result in a third number passed as parameter.
         * @param _result Result of the operation.
         * @param _op2 Righthand side of the operation.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& add(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = mValue + _op2.mValue;
            return _result;
        }

        /**
         * Function for subtraction of two numbers, which assigns the result to the 
         * calling number.
         * @param _op2 Righthand side of the operation
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& sub_assign(const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N)
        {
            mValue = mValue - _op2.mValue;
            return *this;
        }

        /**
         * Function which subtracts the righthand side from this number and puts
         * the result in a third number passed as parameter.
         * @param _result Result of the operation.
         * @param _op2 Righthand side of the operation.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& sub(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = mValue - _op2.mValue;
            return _result;
        }

        /**
         * Function for multiplication of two numbers, which assigns the result to the 
         * calling number.
         * @param _op2 Righthand side of the operation
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& mul_assign(const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N)
        {
            mValue = mValue * _op2.mValue;
            return *this;
        }

        /**
         * Function which multiplicates two numbers and puts the result in a 
         * third number passed as parameter.
         * @param _result Result of the operation.
         * @param _op2 Righthand side of the operation.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& mul(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = mValue * _op2.mValue;
            return _result;
        }

        /**
         * Function for division of two numbers, which assigns the result to the 
         * calling number.
         * @param _op2 Righthand side of the operation
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& div_assign(const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N)
        {
            //assert(_op2 != 0);
            mValue = mValue / _op2.mValue;
            return *this;
        }

        /**
         * Function which divides this number by the righthand side and puts the 
         * result in a third number passed as parameter.
         * @param _result Result of the operation.
         * @param _op2 Righthand side of the operation.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& div(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N) const
        {
            //assert(_op2 != 0);
            _result.mValue = mValue / _op2.mValue;
            return _result;
        }

        /**
         * special operators
         */

        /**
         * Function for the square root of the number, which assigns the result to the 
         * calling number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& sqrt_assign(CARL_RND = CARL_RND::N)
        {
            //assert(*this >= 0);
            mValue = std::sqrt(mValue);
            return *this;
        }

        /**
         * Returns the square root of this number and puts it into a passed result
         * parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& sqrt(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            //assert(*this >= 0);
            _result.mValue = std::sqrt(mValue);
            return _result;
        }

        /**
         * Function for the cubic root of the number, which assigns the result to the 
         * calling number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& cbrt_assign(CARL_RND = CARL_RND::N)
        {
            //assert(*this >= 0);
            mValue = std::cbrt(mValue);
            return *this;
        }

        /**
         * Returns the cubic root of this number and puts it into a passed result
         * parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& cbrt(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            //assert(*this >= 0);
            _result.mValue = std::cbrt(mValue);
            return _result;
        }

        /**
         * Function for the nth root of the number, which assigns the result to the 
         * calling number.
         * @param Degree of the root.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& root_assign(unsigned long int, CARL_RND = CARL_RND::N)
        {
            //assert(*this >= 0);
            /// @todo implement root_assign for FLOAT_T
            LOG_NOTIMPLEMENTED();
            return *this;
        }

        /**
         * Function which calculates the nth root of this number and puts it into a passed result
         * parameter.
         * @param Result.
         * @param Degree of the root.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& root(FLOAT_T<FloatType>&, unsigned long int, CARL_RND = CARL_RND::N) const
        {
            //assert(*this >= 0);
            LOG_NOTIMPLEMENTED();
            /// @todo implement root for FLOAT_T
        }

        /**
         * Function for the nth power of the number, which assigns the result to the 
         * calling number.
         * @param _exp Degree.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& pow_assign(unsigned long int _exp, CARL_RND = CARL_RND::N)
        {
            mValue = std::pow(mValue, _exp);
            return *this;
        }

        /**
         * Function which calculates the power of this number and puts it into a passed result
         * parameter.
         * @param _result Result.
         * @param _deg Degree.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& pow(FLOAT_T<FloatType>& _result, unsigned long int _exp, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::pow(mValue, _exp);
            return _result;
        }

        /**
         * Assigns the number the absolute value of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& abs_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::abs(mValue);
            return *this;
        }

        /**
         * Function which calculates the absolute value of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& abs(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::abs(mValue);
            return _result;
        }

        /**
         * Assigns the number the exponential of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& exp_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::exp(mValue);
            return *this;
        }

        /**
         * Function which calculates the exponential of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& exp(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::exp(mValue);
            return _result;
        }

        /**
         * Assigns the number the sine of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& sin_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::sin(mValue);
            return *this;
        }

        /**
         * Function which calculates the sine of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& sin(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::sin(mValue);
            return _result;
        }

        /**
         * Assigns the number the cosine of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& cos_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::cos(mValue);
            return *this;
        }

        /**
         * Function which calculates the cosine of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& cos(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::cos(mValue);
            return _result;
        }

        /**
         * Assigns the number the logarithm of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& log_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::log(mValue);
            return *this;
        }

        /**
         * Function which calculates the logarithm of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& log(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::log(mValue);
            return _result;
        }
        
        /**
         * Assigns the number the tangent of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& tan_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::tan(mValue);
            return *this;
        }

        /**
         * Function which calculates the tangent of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& tan(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::tan(mValue);
            return _result;
        }

        /**
         * Assigns the number the arcus sine of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& asin_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::asin(mValue);
            return *this;
        }

        /**
         * Function which calculates the arcus sine of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& asin(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::asin(mValue);
            return _result;
        }

        /**
         * Assigns the number the arcus cosine of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& acos_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::acos(mValue);
            return *this;
        }

        /**
         * Function which calculates the arcus cosine of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& acos(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::acos(mValue);
            return _result;
        }

        /**
         * Assigns the number the arcus tangent of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& atan_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::atan(mValue);
            return *this;
        }

        /**
         * Function which calculates the arcus tangent of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& atan(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::atan(mValue);
            return _result;
        }

        /**
         * Assigns the number the hyperbolic sine of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& sinh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::sinh(mValue);
            return *this;
        }

        /**
         * Function which calculates the hyperbolic sine of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& sinh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::sinh(mValue);
            return _result;
        }

        /**
         * Assigns the number the hyperbolic cosine of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& cosh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::cosh(mValue);
            return *this;
        }

        /**
         * Function which calculates the hyperbolic cosine of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& cosh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::cosh(mValue);
            return _result;
        }

        /**
         * Assigns the number the hyperbolic tangent of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& tanh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::tanh(mValue);
            return *this;
        }

        /**
         * Function which calculates the hyperbolic tangent of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& tanh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::tanh(mValue);
            return _result;
        }

        /**
         * Assigns the number the hyperbolic arcus sine of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& asinh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::asinh(mValue);
            return *this;
        }

        /**
         * Function which calculates the hyperbolic arcus sine of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& asinh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::asinh(mValue);
            return _result;
        }

        /**
         * Assigns the number the hyperbolic arcus cosine of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& acosh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::acosh(mValue);
            return *this;
        }

        /**
         * Function which calculates the hyperbolic arcus cosine of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& acosh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::acosh(mValue);
            return _result;
        }

        /**
         * Assigns the number the hyperbolic arcus tangent of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& atanh_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::atanh(mValue);
            return *this;
        }

        /**
         * Function which calculates the hyperbolic arcus tangent of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& atanh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result.mValue = std::atanh(mValue);
            return _result;
        }

        /**
         * Function which calculates the floor of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& floor(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result = std::floor(mValue);
            return _result;
        }

        /**
         * Assigns the number the floor of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& floor_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::floor(mValue);
            return *this;
        }

        /**
         * Function which calculates the ceiling of this number and puts 
         * it into a passed result parameter.
         * @param _result Result.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& ceil(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
        {
            _result = std::ceil(mValue);
            return _result;
        }

        /**
         * Assigns the number the ceiling of this number.
         * @param N Possible rounding direction.
         * @return 
         */
        FLOAT_T<FloatType>& ceil_assign(CARL_RND = CARL_RND::N)
        {
            mValue = std::ceil(mValue);
            return *this;
        }

        /**
         * conversion operators
         */
        
        /**
         * Function which converts the number to a double value.
         * @param N Possible rounding direction.
         * @return 
         */
        double toDouble(CARL_RND = CARL_RND::N) const
        {
            return (double) mValue;
        }

        /**
         * Typecast operators
         */
        
        /**
         * Explicit typecast operator to integer.
         * @return 
         */
        explicit operator int() const
        {
            return (int) mValue;
        }
        
        /**
         * Explicit typecast operator to long.
         * @return 
         */
        explicit operator long() const
        {
            return (long) mValue;
        }
        
        /**
         * Explicit typecast operator to double.
         * @return 
         */
        explicit operator double() const
        {
            return (double) mValue;
        }
        
        /**
         * Output stream operator for numbers of type FLOAT_T.
         * @param ostr Output stream.
         * @param p Number.
         * @return 
         */
        friend std::ostream & operator<<(std::ostream& ostr, const FLOAT_T<FloatType>& p)
        {
            ostr << p.toString();
            return ostr;
        }

        /**
         * Comparison operator which tests for equality of two numbers.
         * @param _lhs Lefthand side of the comparison.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        friend bool operator==(const FLOAT_T<FloatType>& _lhs, const int _rhs)
        {
            return _lhs.mValue == _rhs;
        }

        /**
         * Comparison operator which tests for equality of two numbers.
         * @param _lhs Lefthand side of the comparison.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        friend bool operator==(const int _lhs, const FLOAT_T<FloatType>& _rhs)
        {
            return _rhs == _lhs;
        }

        /**
         * Comparison operator which tests for equality of two numbers.
         * @param _lhs Lefthand side of the comparison.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        friend bool operator==(const FLOAT_T<FloatType>& _lhs, const double _rhs)
        {
            return _lhs.mValue == _rhs;
        }

        /**
         * Comparison operator which tests for equality of two numbers.
         * @param _lhs Lefthand side of the comparison.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        friend bool operator==(const double _lhs, const FLOAT_T<FloatType>& _rhs)
        {
            return _rhs == _lhs;
        }

        /**
         * Comparison operator which tests for equality of two numbers.
         * @param _lhs Lefthand side of the comparison.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        friend bool operator==(const FLOAT_T<FloatType>& _lhs, const float _rhs)
        {
            return _lhs.mValue == _rhs;
        }

        /**
         * Comparison operator which tests for equality of two numbers.
         * @param _lhs Lefthand side of the comparison.
         * @param _rhs Righthand side of the comparison.
         * @return 
         */
        friend bool operator==(const float _lhs, const FLOAT_T<FloatType>& _rhs)
        {
            return _rhs == _lhs;
        }

        /**
         * operations required for the usage of Eigen3
         */

        /**
         * Function required for extension of Eigen3 with FLOAT_T as 
         * a custom type which calculates the complex conjugate.
         * @param x The passed number.
         * @return 
         */
        inline const FLOAT_T<FloatType>& ei_conj(const FLOAT_T<FloatType>& x)
        {
            return x;
        }

        /**
         * Function required for extension of Eigen3 with FLOAT_T as 
         * a custom type which calculates the real part.
         * @param x The passed number.
         * @return 
         */
        inline const FLOAT_T<FloatType>& ei_real(const FLOAT_T<FloatType>& x)
        {
            return x;
        }

        /**
         * Function required for extension of Eigen3 with FLOAT_T as 
         * a custom type which calculates the imaginary part.
         * @param x The passed number.
         * @return 
         */
        inline FLOAT_T<FloatType> ei_imag(const FLOAT_T<FloatType>&)
        {
            return FLOAT_T<FloatType>(0);
        }

        /**
         * Function required for extension of Eigen3 with FLOAT_T as 
         * a custom type which calculates the absolute value.
         * @param x The passed number.
         * @return 
         */
        inline FLOAT_T<FloatType> ei_abs(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.abs(res);
            return res;
        }

        /**
         * Function required for extension of Eigen3 with FLOAT_T as 
         * a custom type which calculates the absolute value (special Eigen3
         * version).
         * @param x The passed number.
         * @return 
         */
        inline FLOAT_T<FloatType> ei_abs2(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.mul(res, x);
            return res;
        }

        /**
         * Function required for extension of Eigen3 with FLOAT_T as 
         * a custom type which calculates the square root.
         * @param x The passed number.
         * @return 
         */
        inline FLOAT_T<FloatType> ei_sqrt(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.sqrt(res);
            return res;
        }

        /**
         * Function required for extension of Eigen3 with FLOAT_T as 
         * a custom type which calculates the exponential.
         * @param x The passed number.
         * @return 
         */
        inline FLOAT_T<FloatType> ei_exp(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.exp(res);
            return res;
        }

        /**
         * Function required for extension of Eigen3 with FLOAT_T as 
         * a custom type which calculates the logarithm.
         * @param x The passed number.
         * @return 
         */
        inline FLOAT_T<FloatType> ei_log(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.log(res);
            return res;
        }

        /**
         * Function required for extension of Eigen3 with FLOAT_T as 
         * a custom type which calculates the sine.
         * @param x The passed number.
         * @return 
         */
        inline FLOAT_T<FloatType> ei_sin(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.sin(res);
            return res;
        }

        /**
         * Function required for extension of Eigen3 with FLOAT_T as 
         * a custom type which calculates the cosine.
         * @param x The passed number.
         * @return 
         */
        inline FLOAT_T<FloatType> ei_cos(const FLOAT_T<FloatType>& x)
        {
            FLOAT_T<FloatType> res;
            x.cos(res);
            return res;
        }

        /**
         * Function required for extension of Eigen3 with FLOAT_T as 
         * a custom type which calculates the power.
         * @param x The passed number.
         * @param y Degree.
         * @return 
         */
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
        
        friend FLOAT_T<FloatType> operator -(const FLOAT_T<FloatType>& _lhs)
        {
            return FLOAT_T<FloatType>(-1*_lhs);
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
        
        FLOAT_T<FloatType> operator-()
        {
            FLOAT_T<FloatType> result = FLOAT_T<FloatType>(*this);
            result *= FLOAT_T<FloatType>(-1);
            return result;
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
    
    template<typename FloatType>
    static FLOAT_T<FloatType> abs(const FLOAT_T<FloatType>& in)
    {
        FLOAT_T<FloatType> result;
        in.abs(result);
        return result;
    }
    
    template<typename FloatType>
    static FLOAT_T<FloatType> log(const FLOAT_T<FloatType>& in)
    {
        FLOAT_T<FloatType> result;
        in.log(result);
        return result;
    }
    
    template<typename FloatType>
    static FLOAT_T<FloatType> sqrt(const FLOAT_T<FloatType>& in)
    {
        FLOAT_T<FloatType> result;
        in.sqrt(result);
        return result;
    }
    
    template<typename FloatType>
    static FLOAT_T<FloatType> floor(const FLOAT_T<FloatType>& in)
    {
        FLOAT_T<FloatType> result;
        in.floor(result);
        return result;
    }
    
    template<typename FloatType>
    static FLOAT_T<FloatType> ceil(const FLOAT_T<FloatType>& in)
    {
        FLOAT_T<FloatType> result;
        in.ceil(result);
        return result;
    }
    
    template<typename FloatType>
    static cln::cl_I getDenom(const FLOAT_T<FloatType>& in)
    {
        return carl::getDenom(carl::rationalize<cln::cl_RA>(in.toDouble()));
    }
    
    template<typename FloatType>
    static cln::cl_I getNum(const FLOAT_T<FloatType>& in)
    {
        return carl::getNum(carl::rationalize<cln::cl_RA>(in.toDouble()));
    }
    
#ifdef USE_MPFR_FLOAT
#include "adaption_float/mpfr_float.tpp"
#endif
}