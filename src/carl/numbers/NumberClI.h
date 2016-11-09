#pragma once

#include "Number.h"
#include "NumberMpz.h"




namespace carl {

#ifdef USE_CLN_NUMBERS

	template<>
	class Number<cln::cl_I> : public BaseNumber<cln::cl_I> {
	public:

		Number(): BaseNumber() {}
		explicit Number(const cln::cl_I& t): BaseNumber(t) {}
		explicit Number(cln::cl_I&& t): BaseNumber(t) {}
		Number(const Number<cln::cl_I>& n): BaseNumber(n) {}
		Number(Number<cln::cl_I>&& n) noexcept : BaseNumber(n) {}
		//explicit Number(const std::string& s) { mData = cln::cl_I(s.c_str()); }
		Number(int n) : BaseNumber(n) {}
		//TODO: this might not be the best way to do it
		Number(long long int n) { mData = cln::cl_I(n); }
		Number(unsigned long long int n) { mData = cln::cl_I(n);}
	

		//TODO: conversion constructors etc
		Number(const Number<mpz_class>& n) : Number(cln::cl_I(n.toString().c_str())) {}


		Number<cln::cl_I>& operator=(const Number<cln::cl_I>& n) {
			this->mData = n.mData;
			return *this;
		}

		template<typename Other>
		Number<cln::cl_I>& operator=(const Other& n) {
			this->mData = n;
			return *this;
		}

		Number<cln::cl_I>& operator=(Number<cln::cl_I>&& n) noexcept {
			this->mData = std::move(n.mData);
			return *this;
		}



		std::string toString(bool _infix = true) const;

	
		//could probably be implemented the same way as for mpq, mpz
		inline bool isZero()  {
			return zerop(mData);
		}


		
		//these 3 methods are the same as for mpq, mpz
		bool isOne()  {
			return mData == carl::constant_one<cln::cl_I>().get();
		}


		inline bool isPositive()  {
			return mData > carl::constant_zero<cln::cl_RA>().get();
		}



		inline bool isNegative()  {
			return mData < carl::constant_zero<cln::cl_RA>().get();
		}





		/**
		 * Check if a number is integral.
		 * As cln::cl_I are always integral, this method returns true.
		 * @param An integer.
		 * @return true.
		 */

		//this is the same as for mpz
		inline bool isInteger()  {
			return true;
		}


		/**
		 * Get the bit size of the representation of a integer.
		 * @param n An integer.
		 * @return Bit size of n.
		 */
		inline std::size_t bitsize()  {
			return cln::integer_length(mData);
		}


		/**
		 * Converts the given integer to a double.
		 * @param n An integer.
		 * @return Double.
		 */
		inline double toDouble()  {
			return cln::double_approx(mData);
		}

		template<typename Integer>
		inline Integer toInt() ;

		/**
		 * Get absolute value of an integer.
		 * @param n An integer.
		 * @return \f$|n|\f$.
		 */
		Number<cln::cl_I> abs() const {
			return Number(cln::abs(mData));
		}

		/*
		 * Calculate the greatest common divisor of two integers.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return Gcd of a and b.
		 */
		inline Number<cln::cl_I> gcd(const Number<cln::cl_I>& b)  {
			return Number(cln::gcd(mData,b.mData));
		}


		/**
		 * Calculate the least common multiple of two integers.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return Lcm of a and b.
		 */
		inline Number<cln::cl_I> lcm(const Number<cln::cl_I>& b)  {
			return Number(cln::lcm(mData,b.mData));
		}




		/**
		 * Calculate the remainder of the integer division.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return \f$a \% b\f$.
		 */
		inline Number<cln::cl_I> mod(const Number<cln::cl_I>& b) {
			return Number(cln::rem(mData, b.mData));
		}


		/**
		 * Divide two integers.
		 * Asserts that the remainder is zero.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return \f$ a / b \f$.
		 */
		inline Number<cln::cl_I> div(const Number<cln::cl_I>& b) {
			assert(cln::mod(mData, b.mData) == 0);
			return Number(cln::exquo(mData, b.mData));
		}




		/**
		 * Divide two integers.
		 * Discards the remainder of the division.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return \f$ a / b \f$.
		 */
		inline Number<cln::cl_I> quotient(const Number<cln::cl_I>& b)
		{
			return Number(cln::exquo(mData - cln::rem(mData, b.mData), b.mData));
		}


		/**
		 * Calculate the remainder of the integer division.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return \f$a \% b\f$.
		 */
		inline Number<cln::cl_I> remainder(const Number<cln::cl_I>& b) {
			return Number(cln::rem(mData, b.mData));
		}


		/**
		 * Divide two integers.
		 * Discards the remainder of the division.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return \f$ a / b \f$.
		 */
		inline Number<cln::cl_I> operator/(const Number<cln::cl_I>& b)
		{
			return this->quotient(b);
		}
		inline Number<cln::cl_I> operator/(const int& rhs) {
			return Number(mData / cln::cl_I(rhs));
		}

		
	};


	//template specializations of member-functions of Number<cln::cl_I>
	template<>
	inline sint Number<cln::cl_I>::toInt<sint>() {
	    assert(mData <= std::numeric_limits<sint>::max());
	    assert(mData >= std::numeric_limits<sint>::min());
	    return cln::cl_I_to_long(mData);
	}
	template<>
	inline uint Number<cln::cl_I>::toInt<uint>() {
	    assert(mData <= std::numeric_limits<uint>::max());
	    assert(mData >= std::numeric_limits<uint>::min());
	    return uint(cln::cl_I_to_long(mData));
	}

#endif


}
