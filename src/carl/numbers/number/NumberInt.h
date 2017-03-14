#pragma once



#include "Number.h"
#include <string>



namespace carl {




	template<>
	class Number<mpz_class>;

	template<>
	class Number<long int> : public BaseNumber<long int,Number<long int>> {
	public:

		Number(): BaseNumber() {}
		explicit Number(const long int t) {mData = t; }
		Number(const Number<long int>& n): BaseNumber(n) {}
		Number(Number<long int>&& n) noexcept : BaseNumber(n) {}
		explicit Number(const std::string& s) { mData = atoi(s.c_str()); }
		/*Number(long long int n) { mData = cln::cl_I(n); }
		Number(unsigned long long int n) { mData = cln::cl_I(n);} */
	 	Number(const Number<mpz_class>& n); 
		


		Number<long int>& operator=(const Number<long int>& n) {
			this->mData = n.mData;
			return *this;
		}

		template<typename Other>
		Number<long int>& operator=(const Other& n) {
			this->mData = n;
			return *this;
		}

		Number<long int>& operator=(Number<long int>&& n) noexcept {
			this->mData = std::move(n.mData);
			return *this;
		}




		std::string toString(bool) const {
			return std::to_string(mData);
		}

	





		/**
		 * Check if a number is integral.
		 * As cln::cl_I are always integral, this method returns true.
		 * @param An integer.
		 * @return true.
		 */

		//this is the same as for mpz
		inline bool isInteger() const  {
			return true;
		}


		/**
		 * Get the bit size of the representation of a integer.
		 * @param n An integer.
		 * @return Bit size of n.
		 */
		inline std::size_t bitsize() const {
			return std::numeric_limits<long long int>::digits;
		}


		/**
		 * Converts the given integer to a double.
		 * @param n An integer.
		 * @return Double.
		 */
		inline double toDouble() const {
			return static_cast<double>(mData);
		}


//TODO: the rest...
/*		template<typename Integer>
		inline Integer toInt() ;
*/
		/**
		 * Get absolute value of an integer.
		 * @param n An integer.
		 * @return \f$|n|\f$.
		 */
		//Number<cln::cl_I> abs() const {
		//	return Number(cln::abs(mData));
		//}

		/*
		 * Calculate the greatest common divisor of two integers.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return Gcd of a and b.
		 */
		//inline Number<cln::cl_I> gcd(const Number<cln::cl_I>& b)  {
		//	return Number(cln::gcd(mData,b.mData));
		//}


		/**
		 * Calculate the least common multiple of two integers.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return Lcm of a and b.
		 */
		//inline Number<cln::cl_I> lcm(const Number<cln::cl_I>& b)  {
		//	return Number(cln::lcm(mData,b.mData));
		//}


		//Number<cln::cl_I> pow(std::size_t exp) const;


		/**
		 * Calculate the remainder of the integer division.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return \f$a \% b\f$.
		 */
		//inline Number<cln::cl_I> mod(const Number<cln::cl_I>& b) {
		//	return Number(cln::rem(mData, b.mData));
		//}


		/**
		 * Divide two integers.
		 * Asserts that the remainder is zero.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return \f$ a / b \f$.
		 */
	/*	inline Number<cln::cl_I> div(const Number<cln::cl_I>& b) {
			assert(cln::mod(mData, b.mData) == 0);
			return Number(cln::exquo(mData, b.mData));
		}

*/


		/**
		 * Divide two integers.
		 * Discards the remainder of the division.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return \f$ a / b \f$.
		 */
	/*	inline Number<cln::cl_I> quotient(const Number<cln::cl_I>& b)
		{
			return Number(cln::exquo(mData - cln::rem(mData, b.mData), b.mData));
		}

*/
		/**
		 * Calculate the remainder of the integer division.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return \f$a \% b\f$.
		 */
	/*	inline Number<cln::cl_I> remainder(const Number<cln::cl_I>& b) {
			return Number(cln::rem(mData, b.mData));
		}

*/
	};

}


	//template specializations of member-functions of Number<cln::cl_I>
/*	template<>
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

*/
