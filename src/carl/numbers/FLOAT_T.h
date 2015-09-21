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
#include <cfloat>
#include <iostream>
#include <assert.h>
#include <math.h>
#include <cmath>
#include <cstddef>
#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
#else
#include <gmpxx.h>
#endif
#ifdef USE_MPFR_FLOAT
#include <mpfr.h>
#endif

#include "typetraits.h"
#ifdef USE_CLN_NUMBERS
#include "adaption_cln/typetraits.h"
#include "adaption_cln/operations.h"
#else
#include "adaption_gmpxx/typetraits.h"
#include "adaption_gmpxx/operations.h"
#endif
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


	enum Str2Double_Error { FLOAT_SUCCESS, FLOAT_OVERFLOW, FLOAT_UNDERFLOW, FLOAT_INCONVERTIBLE };

	inline Str2Double_Error str2double (double &d, char const *s)
	{
		char *end;
		long double  l;
		errno = 0;
		l = strtod(s, &end);
		if ((errno == ERANGE && l == LDBL_MAX) || l > DBL_MAX) {
			return FLOAT_OVERFLOW;
		}
		if ((errno == ERANGE && l == LDBL_MIN) || l < DBL_MIN) {
			return FLOAT_UNDERFLOW;
		}
		if (*s == '\0' || *end != '\0') {
			return FLOAT_INCONVERTIBLE;
		}
		d = double(l);
		return FLOAT_SUCCESS;
	}

	// Usable AlmostEqual function taken from http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
	template<typename Number>
	inline bool AlmostEqual2sComplement(const Number& A, const Number& B, unsigned = 128)
	{
		return A == B;
	}

	template<>
	inline bool AlmostEqual2sComplement<double>(const double& A, const double& B, unsigned maxUlps)
	{
		// Make sure maxUlps is non-negative and small enough that the
		// default NAN won't compare as equal to anything.
		assert(maxUlps > 0 && maxUlps < 4 * 1024 * 1024);
		long long aInt = *(long long*)&A;
		// Make aInt lexicographically ordered as a twos-complement int
		if (aInt < 0)
			aInt = (long long)(0x8000000000000000) - aInt;
		// Make bInt lexicographically ordered as a twos-complement int
		long long bInt = *(long long*)&B;
		if (bInt < 0)
			bInt = (long long)(0x8000000000000000) - bInt;
		unsigned long long intDiff = (unsigned long long)std::abs(aInt - bInt);
		if (intDiff <= maxUlps)
			return true;

		return false;
	}

	/**
	 * Templated wrapper class which allows universal usage of different
	 * IEEE 754 implementations.
	 * For each implementation intended to use it is necessary to implement the
	 * according specialization of this class.
	 */
	template<typename FloatType>
	class FLOAT_T
	{
		static_assert(carl::is_subset_of_integers<FloatType>::value == false, "FLOAT_T may not be used with integers.");
	private:
		FloatType mValue;

	public:

		/**
		 * Default empty constructor, which initializes to zero.
		 */
		FLOAT_T<FloatType>() :
			mValue()
		{
			assert(std::is_floating_point<FloatType>::value);
		}

		/**
		 * Constructor, which takes a double as input and optional rounding, which
		 * can be used, if the underlying fp implementation allows this.
		 * @param _double Value to be initialized.
		 * @param N Possible rounding direction.
		 */
		FLOAT_T<FloatType>(const double _double, const CARL_RND=CARL_RND::N)
		{
			assert(std::is_floating_point<FloatType>::value);
			mValue = _double;
		}

		/**
		 * Constructor, which takes a float as input and optional rounding, which
		 * can be used, if the underlying fp implementation allows this.
		 * @param _float Value to be initialized.
		 * @param N Possible rounding direction.
		 */
		FLOAT_T<FloatType>(const float _float, const CARL_RND=CARL_RND::N)
		{
			assert(std::is_floating_point<FloatType>::value);
			mValue = _float;
		}

		/**
		 * Constructor, which takes an integer as input and optional rounding, which
		 * can be used, if the underlying fp implementation allows this.
		 * @param _int Value to be initialized.
		 * @param N Possible rounding direction.
		 */
		FLOAT_T<FloatType>(const int _int, const CARL_RND=CARL_RND::N)
		{
			assert(std::is_floating_point<FloatType>::value);
			mValue = _int;
		}

		/**
		 * Constructor, which takes an unsigned integer as input and optional rounding, which
		 * can be used, if the underlying fp implementation allows this.
		 * @param _int Value to be initialized.
		 * @param N Possible rounding direction.
		 */
		FLOAT_T<FloatType>(const unsigned _int, const CARL_RND=CARL_RND::N)
		{
			assert(std::is_floating_point<FloatType>::value);
			mValue = _int;
		}

		/**
		 * Constructor, which takes a long as input and optional rounding, which
		 * can be used, if the underlying fp implementation allows this.
		 * @param _long Value to be initialized.
		 * @param N Possible rounding direction.
		 */
		FLOAT_T<FloatType>(const long _long, const CARL_RND=CARL_RND::N)
		{
			assert(std::is_floating_point<FloatType>::value);
			mValue = _long;
		}

		/**
		 * Constructor, which takes an unsigned long as input and optional rounding, which
		 * can be used, if the underlying fp implementation allows this.
		 * @param _long Value to be initialized.
		 * @param N Possible rounding direction.
		 */
		FLOAT_T<FloatType>(const unsigned long _long, const CARL_RND=CARL_RND::N)
		{
			assert(std::is_floating_point<FloatType>::value);
			mValue = _long;
		}

		/**
		 * Copyconstructor which takes a FLOAT_T<FloatType>  and optional rounding
		 * as input, which can be used, if the underlying fp implementation
		 * allows this.
		 * @param _float Value to be initialized.
		 * @param N Possible rounding direction.
		 */
		FLOAT_T<FloatType>(const FLOAT_T<FloatType>& _float, const CARL_RND=CARL_RND::N) : mValue(_float.mValue)
		{
			assert(std::is_floating_point<FloatType>::value);
		}

		FLOAT_T<FloatType>(FLOAT_T<FloatType>&& _float, const CARL_RND=CARL_RND::N) : mValue(_float.value())
		{
			assert(std::is_floating_point<FloatType>::value);
		}

		/**
		 * Constructor, which takes an arbitrary fp type as input and optional rounding, which
		 * can be used, if the underlying fp implementation allows this.
		 * @param val Value to be initialized.
		 * @param N Possible rounding direction.
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
		 * @param N Possible rounding direction.
		 */
		template<typename F, DisableIf< std::is_same<F, FloatType> > = dummy>
		FLOAT_T<FloatType>(const FLOAT_T<F>& _float, const CARL_RND=CARL_RND::N)
		{
			mValue = _float.toDouble();
		}

		FLOAT_T<FloatType>(const std::string& _string, const CARL_RND=CARL_RND::N)
		{
			str2double (mValue, _string);
//			mValue = std::atof(_string.c_str());
		}

		/**
		 * Destructor. Note that for some specializations memory management has to
		 * be included here.
		 */
		~FLOAT_T() { }

		/**
		 * Getter for the raw value contained.
		 * @return Raw value.
		 */
		const FloatType& value() const {
			return mValue;
		}

		/**
		 * If precision is used, this getter returns the acutal precision (default:
		 * 53 bit).
		 * @return Precision.
		 */
		precision_t precision() const
		{
			return 0;
		}

		/**
		 * Allows to set the desired precision. Note: If the value is already
		 * initialized this can change the internal value.
		 * @param Precision in bits.
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& setPrecision(const precision_t&)
		{
			return *this;
		}

		/**
		 * Assignment operator.
		 * @param _rhs Righthand side of the assignment.
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& operator =(const FLOAT_T<FloatType>& _rhs)
		{
			mValue = _rhs.mValue;
			return *this;
		}

		/**
		 * Comparison operator for equality.
		 * @param _rhs Righthand side of the comparison.
		 * @return True if _rhs equals this.
		 */
		bool operator ==(const FLOAT_T<FloatType>& _rhs) const
		{
			//std::cout << "COMPARISON: " << *this << " == " << _rhs << " : " << (mValue == _rhs.mValue) << std::endl;
			//return mValue == _rhs.mValue;
			return AlmostEqual2sComplement(double(mValue), double(_rhs.mValue), 4);
		}

		/**
		 * Comparison operator for inequality.
		 * @param _rhs Righthand side of the comparison.
		 * @return True if _rhs is unequal to this.
		 */
		bool operator !=(const FLOAT_T<FloatType> & _rhs) const
		{
			return mValue != _rhs.mValue;
		}

		/**
		 * Comparison operator for larger than.
		 * @param _rhs Righthand side of the comparison.
		 * @return True if _rhs is larger than this.
		 */
		bool operator>(const FLOAT_T<FloatType> & _rhs) const
		{
			return mValue > _rhs.mValue;
		}

		bool operator>(int _rhs) const
		{
			return mValue > _rhs;
		}

		bool operator>(unsigned _rhs) const
		{
			return mValue > _rhs;
		}


		/**
		 * Comparison operator for less than.
		 * @param _rhs Righthand side of the comparison.
		 * @return  True if _rhs is smaller than this.
		 */
		bool operator<(const FLOAT_T<FloatType> & _rhs) const
		{
			return mValue < _rhs.mValue;
		}

		bool operator<(int _rhs) const
		{
			return mValue < _rhs;
		}

		bool operator<(unsigned _rhs) const
		{
			return mValue < _rhs;
		}

		/**
		 * Comparison operator for less or equal than.
		 * @param _rhs Righthand side of the comparison.
		 * @return True if _rhs is larger or equal than this.
		 */
		bool operator <=(const FLOAT_T<FloatType> & _rhs) const
		{
			return mValue <= _rhs.mValue;
		}

		/**
		 * Comparison operator for larger or equal than.
		 * @param _rhs Righthand side of the comparison.
		 * @return True if _rhs is smaller or equal than this.
		 */
		bool operator >=(const FLOAT_T<FloatType> & _rhs) const
		{
			return mValue >= _rhs.mValue;
		}

		/**
		 * Function for addition of two numbers, which assigns the result to the
		 * calling number.
		 * @param _op2 Righthand side of the operation
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
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
		 * @return Reference to this.
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
		 * @return Reference to the result.
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
		 * @return Reference to this.
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
		 * @return Reference to the result.
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
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& div_assign(const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N)
		{
			assert(_op2 != 0);
			mValue = mValue / _op2.mValue;
			return *this;
		}

		/**
		 * Function which divides this number by the righthand side and puts the
		 * result in a third number passed as parameter.
		 * @param _result Result of the operation.
		 * @param _op2 Righthand side of the operation.
		 * @param N Possible rounding direction.
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& div(FLOAT_T<FloatType>& _result, const FLOAT_T<FloatType>& _op2, CARL_RND = CARL_RND::N) const
		{
			assert(_op2 != 0);
			_result.mValue = mValue / _op2.mValue;
			return _result;
		}

		/**
		 * Function for the square root of the number, which assigns the result to the
		 * calling number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& sqrt_assign(CARL_RND = CARL_RND::N)
		{
			assert(*this >= 0);
			mValue = std::sqrt(mValue);
			return *this;
		}

		/**
		 * Returns the square root of this number and puts it into a passed result
		 * parameter.
		 * @param _result Result.
		 * @param N Possible rounding direction.
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& sqrt(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			assert(mValue >= 0);
			_result.mValue = std::sqrt(mValue);
			return _result;
		}

		/**
		 * Function for the cubic root of the number, which assigns the result to the
		 * calling number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& cbrt_assign(CARL_RND = CARL_RND::N)
		{
			assert(*this >= 0);
			mValue = std::cbrt(mValue);
			return *this;
		}

		/**
		 * Returns the cubic root of this number and puts it into a passed result
		 * parameter.
		 * @param _result Result.
		 * @param N Possible rounding direction.
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& cbrt(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			assert(*this >= 0);
			_result.mValue = std::cbrt(mValue);
			return _result;
		}

		/**
		 * Function for the nth root of the number, which assigns the result to the
		 * calling number.
		 * @param Degree of the root.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& root_assign(unsigned long int, CARL_RND = CARL_RND::N)
		{
			assert(*this >= 0);
			/// @todo implement root_assign for FLOAT_T
			CARL_LOG_NOTIMPLEMENTED();
			return *this;
		}

		/**
		 * Function which calculates the nth root of this number and puts it into a passed result
		 * parameter.
		 * @param Result.
		 * @param Degree of the root.
		 * @param N Possible rounding direction.
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& root(FLOAT_T<FloatType>&, unsigned long int, CARL_RND = CARL_RND::N) const
		{
			assert(*this >= 0);
			CARL_LOG_NOTIMPLEMENTED();
			/// @todo implement root for FLOAT_T
		}

		/**
		 * Function for the nth power of the number, which assigns the result to the
		 * calling number.
		 * @param _exp Exponent.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& pow_assign(unsigned _exp, CARL_RND = CARL_RND::N)
		{
			mValue = std::pow(mValue, _exp);
			return *this;
		}

		/**
		 * Function which calculates the power of this number and puts it into a passed result
		 * parameter.
		 * @param _result Result.
		 * @param _exp Exponent.
		 * @param N Possible rounding direction.
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& pow(FLOAT_T<FloatType>& _result, unsigned _exp, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::pow(mValue, _exp);
			return _result;
		}

		/**
		 * Assigns the number the absolute value of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& abs(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::abs(mValue);
			return _result;
		}

		/**
		 * Assigns the number the exponential of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& exp(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::exp(this->mValue);
			return _result;
		}

		/**
		 * Assigns the number the sine of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& sin(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::sin(mValue);
			return _result;
		}

		/**
		 * Assigns the number the cosine of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& cos(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::cos(mValue);
			return _result;
		}

		/**
		 * Assigns the number the logarithm of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& log(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::log(mValue);
			return _result;
		}

		/**
		 * Assigns the number the tangent of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& tan(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::tan(mValue);
			return _result;
		}

		/**
		 * Assigns the number the arcus sine of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& asin(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::asin(mValue);
			return _result;
		}

		/**
		 * Assigns the number the arcus cosine of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& acos(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::acos(mValue);
			return _result;
		}

		/**
		 * Assigns the number the arcus tangent of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& atan(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::atan(mValue);
			return _result;
		}

		/**
		 * Assigns the number the hyperbolic sine of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& sinh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::sinh(mValue);
			return _result;
		}

		/**
		 * Assigns the number the hyperbolic cosine of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& cosh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::cosh(mValue);
			return _result;
		}

		/**
		 * Assigns the number the hyperbolic tangent of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& tanh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::tanh(mValue);
			return _result;
		}

		/**
		 * Assigns the number the hyperbolic arcus sine of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& asinh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::asinh(mValue);
			return _result;
		}

		/**
		 * Assigns the number the hyperbolic arcus cosine of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& acosh(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result.mValue = std::acosh(mValue);
			return _result;
		}

		/**
		 * Assigns the number the hyperbolic arcus tangent of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& floor(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result = std::floor(mValue);
			return _result;
		}

		/**
		 * Assigns the number the floor of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
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
		 * @return Reference to the result.
		 */
		FLOAT_T<FloatType>& ceil(FLOAT_T<FloatType>& _result, CARL_RND = CARL_RND::N) const
		{
			_result = std::ceil(mValue);
			return _result;
		}

		/**
		 * Assigns the number the ceiling of this number.
		 * @param N Possible rounding direction.
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& ceil_assign(CARL_RND = CARL_RND::N)
		{
			mValue = std::ceil(mValue);
			return *this;
		}


		/**
		 * Function which converts the number to a double value.
		 * @param N Possible rounding direction.
		 * @return Double representation of this
		 */
		double toDouble(CARL_RND = CARL_RND::N) const
		{
			return (double) mValue;
		}


		/**
		 * Explicit typecast operator to integer.
		 * @return Integer representation of this.
		 */
		explicit operator int() const
		{
			return (int) mValue;
		}

		/**
		 * Explicit typecast operator to long.
		 * @return Long representation of this.
		 */
		explicit operator long() const
		{
			return (long) mValue;
		}

		/**
		 * Explicit typecast operator to double.
		 * @return Double representation of this.
		 */
		explicit operator double() const
		{
			return (double) mValue;
		}

		/**
		 * Output stream operator for numbers of type FLOAT_T.
		 * @param ostr Output stream.
		 * @param p Number.
		 * @return Reference to the ostream.
		 */
		friend std::ostream& operator<<(std::ostream& ostr, const FLOAT_T<FloatType>& p)
		{
			ostr << p.toString();
			return ostr;
		}

		/**
		 * Comparison operator which tests for equality of two numbers.
		 * @param _lhs Lefthand side of the comparison.
		 * @param _rhs Righthand side of the comparison.
		 * @return True if _lhs equals _rhs.
		 */
		friend bool operator==(const FLOAT_T<FloatType>& _lhs, const int _rhs)
		{
			return _lhs.mValue == _rhs;
		}

		/**
		 * Comparison operator which tests for equality of two numbers.
		 * @param _lhs Lefthand side of the comparison.
		 * @param _rhs Righthand side of the comparison.
		 * @return True if _lhs equals _rhs.
		 */
		friend bool operator==(const int _lhs, const FLOAT_T<FloatType>& _rhs)
		{
			return _rhs == _lhs;
		}

		/**
		 * Comparison operator which tests for equality of two numbers.
		 * @param _lhs Lefthand side of the comparison.
		 * @param _rhs Righthand side of the comparison.
		 * @return True if _lhs equals _rhs.
		 */
		friend bool operator==(const FLOAT_T<FloatType>& _lhs, const double _rhs)
		{
			return _lhs.mValue == _rhs;
		}

		/**
		 * Comparison operator which tests for equality of two numbers.
		 * @param _lhs Lefthand side of the comparison.
		 * @param _rhs Righthand side of the comparison.
		 * @return True if _lhs equals _rhs.
		 */
		friend bool operator==(const double _lhs, const FLOAT_T<FloatType>& _rhs)
		{
			return _rhs == _lhs;
		}

		/**
		 * Comparison operator which tests for equality of two numbers.
		 * @param _lhs Lefthand side of the comparison.
		 * @param _rhs Righthand side of the comparison.
		 * @return True if _lhs equals _rhs.
		 */
		friend bool operator==(const FLOAT_T<FloatType>& _lhs, const float _rhs)
		{
			return _lhs.mValue == _rhs;
		}

		/**
		 * Comparison operator which tests for equality of two numbers.
		 * @param _lhs Lefthand side of the comparison.
		 * @param _rhs Righthand side of the comparison.
		 * @return True if _lhs equals _rhs.
		 */
		friend bool operator==(const float _lhs, const FLOAT_T<FloatType>& _rhs)
		{
			return _rhs == _lhs;
		}

		/**
		 * Function required for extension of Eigen3 with FLOAT_T as
		 * a custom type which calculates the complex conjugate.
		 * @param x The passed number.
		 * @return Reference to x.
		 */
		inline const FLOAT_T<FloatType>& ei_conj(const FLOAT_T<FloatType>& x)
		{
			return x;
		}

		/**
		 * Function required for extension of Eigen3 with FLOAT_T as
		 * a custom type which calculates the real part.
		 * @param x The passed number.
		 * @return Reference to x.
		 */
		inline const FLOAT_T<FloatType>& ei_real(const FLOAT_T<FloatType>& x)
		{
			return x;
		}

		/**
		 * Function required for extension of Eigen3 with FLOAT_T as
		 * a custom type which calculates the imaginary part.
		 * @param x The passed number.
		 * @return Zero.
		 */
		inline FLOAT_T<FloatType> ei_imag(const FLOAT_T<FloatType>&)
		{
			return FLOAT_T<FloatType>(0);
		}

		/**
		 * Function required for extension of Eigen3 with FLOAT_T as
		 * a custom type which calculates the absolute value.
		 * @param x The passed number.
		 * @return Number which holds the absolute value of x.
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
		 * @return Number which holds the absolute value of x according to abs2 of Eigen3.
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
		 * @return Number which holds the square root of x.
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
		 * @return Number which holds the exponential of x.
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
		 * @return Number which holds the logarithm of x.
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
		 * @return Number which holds the sine of x.
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
		 * @return Number which holds the cosine of x.
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
		 * @return Number which holds the power of x of degree y.
		 */
		inline FLOAT_T<FloatType> ei_pow(const FLOAT_T<FloatType>& x, FLOAT_T<FloatType> y)
		{
			FLOAT_T<FloatType> res;
			x.pow(res, unsigned(y));
			return res;
		}

		/**
		 * Operator for addition of two numbers
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator +(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
		{
			return FLOAT_T<FloatType>(_lhs.mValue + _rhs.mValue);
		}

		/**
		 * Operator for addition of two numbers, the righthand side type is the
		 * underlying type.
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator +(const FLOAT_T<FloatType>& _lhs, const FloatType& _rhs)
		{
			return FLOAT_T<FloatType>(_lhs.mValue + _rhs);
		}

		/**
		 * Operator for addition of two numbers, the lefthand side is the underlying
		 * type.
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator +(const FloatType& _lhs, const FLOAT_T<FloatType>& _rhs)
		{
			return _rhs + _lhs;
		}

		/**
		 * Operator for subtraction of two numbers
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator -(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
		{
			return FLOAT_T<FloatType>(_lhs.mValue - _rhs.mValue);
		}

		/**
		 * Operator for addition of two numbers, the righthand side is the underlying
		 * type.
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator -(const FLOAT_T<FloatType>& _lhs, const FloatType& _rhs)
		{
			return FLOAT_T<FloatType>(_lhs.mValue - _rhs);
		}

		/**
		 * Operator for subtraction of two numbers, the lefthand side is the underlying
		 * type.
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator -(const FloatType& _lhs, const FLOAT_T<FloatType>& _rhs)
		{
			return FLOAT_T<FloatType>(_lhs - _rhs.mValue);
		}

		/**
		 * Operator for unary negation of a number.
		 * @param _lhs Lefthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator -(const FLOAT_T<FloatType>& _lhs)
		{
			return FLOAT_T<FloatType>(-1)*_lhs;
		}

		/**
		 * Operator for addition of two numbers.
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator *(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
		{
			return FLOAT_T<FloatType>(_lhs.mValue * _rhs.mValue);
		}

		/**
		 * Operator for multiplication of two numbers, the righthand side is the underlying
		 * type.
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator *(const FLOAT_T<FloatType>& _lhs, const FloatType& _rhs)
		{
			return FLOAT_T<FloatType>(_lhs.mValue * _rhs);
		}

		/**
		 * Operator for addition of two numbers, the lefthand side is the underlying
		 * type.
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator *(const FloatType& _lhs, const FLOAT_T<FloatType>& _rhs)
		{
			return FLOAT_T<FloatType>(_lhs * _rhs.mValue);
		}

		/**
		 * Operator for addition of two numbers.
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator /(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
		{
			assert(_rhs != 0);
			return FLOAT_T<FloatType>(_lhs.mValue / _rhs.mValue);
		}

		/**
		 * Operator for division of two numbers, the righthand side is the underlying
		 * type.
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator /(const FLOAT_T<FloatType>& _lhs, const FloatType& _rhs)
		{
			assert(_rhs != 0);
			return FLOAT_T<FloatType>(_lhs.mValue / _rhs);
		}

		/**
		 * Operator for division of two numbers, the lefthand side is the underlying
		 * type.
		 * @param _lhs Lefthand side.
		 * @param _rhs Righthand side.
		 * @return Number which holds the result.
		 */
		friend FLOAT_T<FloatType> operator /(const FloatType& _lhs, const FLOAT_T<FloatType>& _rhs)
		{
			assert(_rhs != 0);
			return FLOAT_T<FloatType>(_lhs / _rhs.mValue);
		}

		/**
		 * Operator which increments this number by one.
		 * @param _num
		 * @return Reference to _num.
		 */
		friend FLOAT_T<FloatType>& operator ++(FLOAT_T<FloatType>& _num)
		{
			_num.mValue += 1;
			return _num;
		}

		/**
		 * Operator which decrements this number by one.
		 * @param _num
		 * @return Reference to _num.
		 */
		friend FLOAT_T<FloatType>& operator --(FLOAT_T<FloatType>& _num)
		{
			_num.mValue -= 1;
			return _num;
		}

		/**
		 * Operator which adds the righthand side to this.
		 * @param _rhs
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& operator +=(const FLOAT_T<FloatType>& _rhs)
		{
			mValue = mValue + _rhs.mValue;
			return *this;
		}

		/**
		 * Operator which adds the righthand side of the underlying type to this.
		 * @param _rhs
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& operator +=(const FloatType& _rhs)
		{
			mValue = mValue + _rhs;
			return *this;
		}

		/**
		 * Operator which subtracts the righthand side from this.
		 * @param _rhs
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& operator -=(const FLOAT_T<FloatType>& _rhs)
		{
			mValue = mValue - _rhs.mValue;
			return *this;
		}

		/**
		 * Operator which subtracts the righthand side of the underlying type from this.
		 * @param _rhs
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& operator -=(const FloatType& _rhs)
		{
			mValue = mValue - _rhs;
			return *this;
		}

		/**
		 * Operator for unary negation of this number.
		 * @return Number which holds the negated original number.
		 */
		FLOAT_T<FloatType> operator-()
		{
			FLOAT_T<FloatType> result = FLOAT_T<FloatType>(*this);
			result *= FLOAT_T<FloatType>(-1);
			return result;
		}

		/**
		 * Operator which multiplicates this number by the righthand side.
		 * @param _rhs
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& operator *=(const FLOAT_T<FloatType>& _rhs)
		{
			mValue = mValue * _rhs.mValue;
			return *this;
		}

		/**
		 * Operator which multiplicates this number by the righthand side of the
		 * underlying type.
		 * @param _rhs
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& operator *=(const FloatType& _rhs)
		{
			mValue = mValue * _rhs;
			return *this;
		}

		/**
		 * Operator which divides this number by the righthand side.
		 * @param _rhs
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& operator /=(const FLOAT_T<FloatType>& _rhs)
		{
			mValue = mValue / _rhs.mValue;
			return *this;
		}

		/**
		 * Operator which divides this number by the righthand side of the underlying
		 * type.
		 * @param _rhs
		 * @return Reference to this.
		 */
		FLOAT_T<FloatType>& operator /=(const FloatType& _rhs)
		{
			mValue = mValue / _rhs;
			return *this;
		}

		/**
		 * Method which converts this number to a string.
		 * @return String representation of this number.
		 */
		std::string toString() const
		{
			return std::to_string(mValue);
		}
	};

	template<typename FloatType>
	inline bool isInteger(const FLOAT_T<FloatType>&) {
	return false;
	}

	/**
	 * Implements the division which assumes that there is no remainder.
	 * @param _lhs
	 * @param _rhs
	 * @return Number which holds the result.
	 */
	template<typename FloatType>
	inline FLOAT_T<FloatType> div(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
	{
		// TODO
		FLOAT_T<FloatType> result;
		result = _lhs / _rhs;
		return result;
	}

	/**
	 * Implements the division with remainder.
	 * @param _lhs
	 * @param _rhs
	 * @return Number which holds the result.
	 */
	template<typename FloatType>
	inline FLOAT_T<FloatType> quotient(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
	{
		// TODO
		FLOAT_T<FloatType> result;
		result = _lhs / _rhs;
		return result;
	}

	/**
	 * Casts the FLOAT_T to an arbitrary integer type which has a constructor for
	 * a native int.
	 * @param _float
	 * @return Integer type which holds floor(_float).
	 */
	template<typename Integer, typename FloatType>
	inline Integer toInt(const FLOAT_T<FloatType>& _float)
	{
		Integer result = (int)_float;
		return result;
	}

	template<typename FloatType>
	inline double toDouble(const FLOAT_T<FloatType>& _float)
	{
		double result = (double)_float;
		return result;
	}

	/**
	 * Method which returns the absolute value of the passed number.
	 * @param _in Number.
	 * @return Number which holds the result.
	 */
	template<typename FloatType>
	inline FLOAT_T<FloatType> abs(const FLOAT_T<FloatType>& _in)
	{
		FLOAT_T<FloatType> result;
		_in.abs(result);
		return result;
	}

	/**
	 * Method which returns the logarithm of the passed number.
	 * @param _in Number.
	 * @return Number which holds the result.
	 */
	template<typename FloatType>
	inline FLOAT_T<FloatType> log(const FLOAT_T<FloatType>& _in)
	{
		FLOAT_T<FloatType> result;
		_in.log(result);
		return result;
	}

	/**
	 * Method which returns the square root of the passed number.
	 * @param _in Number.
	 * @return Number which holds the result.
	 */
	template<typename FloatType>
	inline FLOAT_T<FloatType> sqrt(const FLOAT_T<FloatType>& _in)
	{
		FLOAT_T<FloatType> result;
		_in.sqrt(result);
		return result;
	}

	template<typename FloatType>
	inline FLOAT_T<FloatType> pow(const FLOAT_T<FloatType>& _in, size_t _exp)
	{
		FLOAT_T<FloatType> result;
		_in.pow(result, _exp);
		return result;
	}

	template<typename FloatType>
	inline FLOAT_T<FloatType> sin(const FLOAT_T<FloatType>& _in)
	{
		FLOAT_T<FloatType> result;
		_in.sin(result);
		return result;
	}

	template<typename FloatType>
	inline FLOAT_T<FloatType> cos(const FLOAT_T<FloatType>& _in)
	{
		FLOAT_T<FloatType> result;
		_in.cos(result);
		return result;
	}

	template<typename FloatType>
	inline FLOAT_T<FloatType> asin(const FLOAT_T<FloatType>& _in)
	{
		FLOAT_T<FloatType> result;
		_in.asin(result);
		return result;
	}

	template<typename FloatType>
	inline FLOAT_T<FloatType> acos(const FLOAT_T<FloatType>& _in)
	{
		FLOAT_T<FloatType> result;
		_in.acos(result);
		return (result);
	}

	template<typename FloatType>
	inline FLOAT_T<FloatType> atan(const FLOAT_T<FloatType>& _in)
	{
		FLOAT_T<FloatType> result;
		_in.atan(result);
		return result;
	}

	/**
	 * Method which returns the next smaller integer of this number or the number
	 * itself, if it is already an integer.
	 * @param _in Number.
	 * @return Number which holds the result.
	 */
	template<typename FloatType>
	inline FLOAT_T<FloatType> floor(const FLOAT_T<FloatType>& _in)
	{
		FLOAT_T<FloatType> result;
		_in.floor(result);
		return result;
	}

	/**
	 * Method which returns the next larger integer of the passed number or the
	 * number itself, if it is already an integer.
	 * @param _in Number.
	 * @return Number which holds the result.
	 */
	template<typename FloatType>
	inline FLOAT_T<FloatType> ceil(const FLOAT_T<FloatType>& _in)
	{
		FLOAT_T<FloatType> result;
		_in.ceil(result);
		return result;
	}

	template<>
	inline FLOAT_T<double> rationalize<FLOAT_T<double>>(double _in)
	{
		return FLOAT_T<double>(_in);
	}

	template<>
	inline FLOAT_T<float> rationalize<FLOAT_T<float>>(float _in)
	{
		return FLOAT_T<float>(_in);
	}


	#ifdef USE_CLN_NUMBERS
	/**
	 * Implicitly converts the number to a rational and returns the denominator.
	 * @param _in Number.
	 * @return Cln interger which holds the result.
	 */
	template<typename FloatType>
	inline cln::cl_I getDenom(const FLOAT_T<FloatType>& _in)
	{
		return carl::getDenom(carl::rationalize<cln::cl_RA>(_in.toDouble()));
	}

	/**
	 * Implicitly converts the number to a rational and returns the nominator.
	 * @param _in Number.
	 * @return Cln interger which holds the result.
	 */
	template<typename FloatType>
	inline cln::cl_I getNum(const FLOAT_T<FloatType>& _in)
	{
		return carl::getNum(carl::rationalize<cln::cl_RA>(_in.toDouble()));
	}
	#else
	/**
	 * Implicitly converts the number to a rational and returns the denominator.
	 * @param _in Number.
	 * @return GMP interger which holds the result.
	 */
	template<typename FloatType>
	inline mpz_class getDenom(const FLOAT_T<FloatType>& _in)
	{
		return carl::getDenom(carl::rationalize<mpq_class>(_in.toDouble()));
	}

	/**
	 * Implicitly converts the number to a rational and returns the nominator.
	 * @param _in Number.
	 * @return GMP interger which holds the result.
	 */
	template<typename FloatType>
	inline mpz_class getNum(const FLOAT_T<FloatType>& _in)
	{
		return carl::getNum(carl::rationalize<mpq_class>(_in.toDouble()));
	}
	#endif

	template<typename FloatType>
	inline bool isZero(const FLOAT_T<FloatType>& _in) {
		return _in.value() == 0;
	}

	template<typename FloatType>
	inline bool isInfinity(const FLOAT_T<FloatType>& _in) {
		return _in.value() == std::numeric_limits<FloatType>::infinity();
	}

	template<typename FloatType>
	inline bool isNan(const FLOAT_T<FloatType>& _in) {
		return _in.value() == std::numeric_limits<FloatType>::quiet_NaN();
	}

	template<>
	inline bool AlmostEqual2sComplement<FLOAT_T<double>>(const FLOAT_T<double>& A, const FLOAT_T<double>& B, unsigned maxUlps)
	{
		return AlmostEqual2sComplement<double>(A.value(), B.value(), maxUlps);
	}

} // namespace

namespace std{

	template<typename Number>
	class numeric_limits<carl::FLOAT_T<Number>>
	{
	public:
		static const bool is_specialized	= true;
		static const bool is_signed			= true;
		static const bool is_integer		= false;
		static const bool is_exact			= false;
		static const int  radix				= 2;    

		static const bool has_infinity		= true;
		static const bool has_quiet_NaN		= true;
		static const bool has_signaling_NaN	= true;

		static const bool is_iec559			= true;	// = IEEE 754
		static const bool is_bounded		= true;
		static const bool is_modulo			= false;
		static const bool traps				= true;
		static const bool tinyness_before	= true;

		static const float_denorm_style has_denorm  = denorm_absent;
		
		inline static carl::FLOAT_T<Number> (min)() { return carl::FLOAT_T<Number>(std::numeric_limits<Number>::min()); }
		inline static carl::FLOAT_T<Number> (max)() { return carl::FLOAT_T<Number>(std::numeric_limits<Number>::max()); }
		inline static carl::FLOAT_T<Number> lowest() { return carl::FLOAT_T<Number>(std::numeric_limits<Number>::lowest()); }

		// Returns smallest eps such that 1 + eps != 1 (classic machine epsilon)
		inline static carl::FLOAT_T<Number> epsilon() {  return carl::FLOAT_T<Number>(std::numeric_limits<Number>::epsilon()); }

		inline static carl::FLOAT_T<Number> round_error() { return carl::FLOAT_T<Number>(std::numeric_limits<Number>::round_error()); }
		
		inline static const carl::FLOAT_T<Number> infinity() { return carl::FLOAT_T<Number>(std::numeric_limits<Number>::infinity()); }
		
		inline static const carl::FLOAT_T<Number> quiet_NaN() { return carl::FLOAT_T<Number>(std::numeric_limits<Number>::quiet_NaN()); }
		inline static const carl::FLOAT_T<Number> signaling_NaN() { return carl::FLOAT_T<Number>(std::numeric_limits<Number>::signaling_NaN()); }
		inline static const carl::FLOAT_T<Number> denorm_min() { return carl::FLOAT_T<Number>(std::numeric_limits<Number>::denorm_min()); }

		static const int min_exponent = std::numeric_limits<Number>::min_exponent;
		static const int max_exponent = std::numeric_limits<Number>::max_exponent;
		static const int min_exponent10 = std::numeric_limits<Number>::min_exponent10; 
		static const int max_exponent10 = std::numeric_limits<Number>::max_exponent10; 

		inline static float_round_style round_style() { return std::numeric_limits<Number>::round_style; }

		inline static int digits() { return std::numeric_limits<Number>::digits; }
		inline static int digits10() { return std::numeric_limits<Number>::digits10; }
		inline static int max_digits10(mp_prec_t precision = carl::FLOAT_T<Number>::get_default_prec()) { return std::numeric_limits<Number>::max_digits10; }
	};
}


#include "adaption_float/mpfr_float.tpp"
