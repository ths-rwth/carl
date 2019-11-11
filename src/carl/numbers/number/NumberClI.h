#pragma once

#include "Number.h"

namespace carl {

#ifdef USE_CLN_NUMBERS


	template<>
	class Number<mpz_class>;

	template<>
	class Number<cln::cl_I> : public BaseNumber<cln::cl_I,Number<cln::cl_I>> {
	public:

		Number() = default;
		Number(const Number<cln::cl_I>& n) = default;
		Number(Number<cln::cl_I>&& n) = default;

		explicit Number(const cln::cl_I& t): BaseNumber(t) {}
		explicit Number(cln::cl_I&& t): BaseNumber(std::move(t)) {}
		explicit Number(const std::string& s) { mData = cln::cl_I(s.c_str()); }
		Number(int n) : BaseNumber(n) {} // NOLINT
		Number(long long int n) { mData = cln::cl_I(n); } // NOLINT
		Number(unsigned long long int n) { mData = cln::cl_I(n);} // NOLINT
		explicit Number(const Number<mpz_class>& n);
		
		Number<cln::cl_I>& operator=(const Number<cln::cl_I>& n) = default;
		Number<cln::cl_I>& operator=(Number<cln::cl_I>&& n) = default;

		template<typename Other>
		Number<cln::cl_I>& operator=(const Other& n) {
			mData = n;
			return *this;
		}


		std::string toString(bool _infix = true) const;

		/**
		 * Check if a number is integral.
		 * As cln::cl_I are always integral, this method returns true.
		 * @return true.
		 */

		//this is the same as for mpz
		inline bool isInteger() const  {
			return true;
		}


		/**
		 * Get the bit size of the representation of a integer.
		 * @return Bit size of n.
		 */
		inline std::size_t bitsize() const  {
			return cln::integer_length(mData);
		}


		/**
		 * Converts the given integer to a double.
		 * @return Double.
		 */
		inline double toDouble() const  {
			return cln::double_approx(mData);
		}

		template<typename Integer>
		inline Integer toInt() const;

		/**
		 * Get absolute value of an integer.
		 * @return \f$|n|\f$.
		 */
		Number<cln::cl_I> abs() const {
			return Number(cln::abs(mData));
		}

		/*
		 * Calculate the greatest common divisor of two integers.
		 * @param b Second argument.
		 * @return Gcd of a and b.
		 */
		inline Number<cln::cl_I> gcd(const Number<cln::cl_I>& b) const  {
			return Number(cln::gcd(mData,b.mData));
		}


		/**
		 * Calculate the least common multiple of two integers.
		 * @param b Second argument.
		 * @return Lcm of a and b.
		 */
		inline Number<cln::cl_I> lcm(const Number<cln::cl_I>& b) const {
			return Number(cln::lcm(mData,b.mData));
		}


		Number<cln::cl_I> pow(std::size_t exp) const;


		/**
		 * Calculate the remainder of the integer division.
		 * @param b Second argument.
		 * @return \f$a \% b\f$.
		 */
		inline Number<cln::cl_I> mod(const Number<cln::cl_I>& b) const {
			return Number(cln::rem(mData, b.mData));
		}


		/**
		 * Divide two integers.
		 * Asserts that the remainder is zero.
		 * @param b Second argument.
		 * @return \f$ a / b \f$.
		 */
		inline Number<cln::cl_I> div(const Number<cln::cl_I>& b) const {
			assert(cln::mod(mData, b.mData) == 0);
			return Number(cln::exquo(mData, b.mData));
		}




		/**
		 * Divide two integers.
		 * Discards the remainder of the division.
		 * @param b Second argument.
		 * @return \f$ a / b \f$.
		 */
		inline Number<cln::cl_I> quotient(const Number<cln::cl_I>& b) const
		{
			return Number(cln::exquo(mData - cln::rem(mData, b.mData), b.mData));
		}


		/**
		 * Calculate the remainder of the integer division.
		 * @param b Second argument.
		 * @return \f$a \% b\f$.
		 */
		inline Number<cln::cl_I> remainder(const Number<cln::cl_I>& b) const {
			return Number(cln::rem(mData, b.mData));
		}


	};


	//template specializations of member-functions of Number<cln::cl_I>
	template<>
	inline sint Number<cln::cl_I>::toInt<sint>() const {
	    assert(mData <= std::numeric_limits<sint>::max());
	    assert(mData >= std::numeric_limits<sint>::min());
	    return cln::cl_I_to_long(mData);
	}
	template<>
	inline uint Number<cln::cl_I>::toInt<uint>() const {
	    assert(mData <= std::numeric_limits<uint>::max());
	    assert(mData >= std::numeric_limits<uint>::min());
	    return uint(cln::cl_I_to_long(mData));
	}

#endif


}
