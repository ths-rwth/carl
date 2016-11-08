#pragma once



#include "Number.h"
#include "NumberClI.h"



namespace carl {

	template<>
	class Number<mpz_class> : public BaseNumber<mpz_class> {
	public:
	

		Number(): BaseNumber() {}
		explicit Number(const mpz_class& t): BaseNumber(t) {}
		explicit Number(mpz_class&& t): BaseNumber(t) {}
		Number(const Number<mpz_class>& n): BaseNumber(n) {}
		Number(Number<mpz_class>&& n) noexcept : BaseNumber(n) {}
		Number(long long int n) : BaseNumber(n) {}
		Number(unsigned long long int n): BaseNumber(n) {}
		Number(const std::string& s) : BaseNumber(s) {}

#ifdef USE_CLN_NUMBERS
		//Number(const Number<cln::cl_I>& n) : Number(n.toString()) {}
#endif


	
		
		inline bool isZero() const {
			return constant_zero<mpz_class>::get() == mData;
		}

		bool isOne() const {
			return constant_one<mpz_class>::get() == mData;
		}

		inline bool isPositive() const {
			return mData > carl::constant_zero<mpz_class>().get();
		}

		inline bool isNegative() const {
			return mData < carl::constant_zero<mpz_class>().get();
		}
		
		//getNum, getDenom removed (they don't make sense here, but could be implemented)
		
		inline bool isInteger() const {
			return true;
		}

		/**
		 * Get the bit size of the representation of a integer.
		 * @param n An integer.
		 * @return Bit size of n.
		 */
		inline std::size_t bitsize() const {
			return mpz_sizeinbase(mData.__get_mp(),2);
		}


		inline double toDouble() const {
			return mData.get_d();
		}

		//specializations of this are below, outside the class
		template<typename Integer>
		Integer toInt();
		

		/**
		 * Basic Operators
		 *
		 * The following functions implement simple operations on the given numbers.
		 */

		inline Number<mpz_class> abs() const {
			mpz_class res;
			mpz_abs(res.get_mpz_t(), mData.get_mpz_t());
			return Number(res);
		}
		
		//TODO: rounding and rationalize needed for mpz? probably not...
	


		inline Number<mpz_class> gcd(const Number<mpz_class>& b) const {
			mpz_class res;
			mpz_gcd(res.get_mpz_t(), mData.get_mpz_t(), b.mData.get_mpz_t());
			return Number(res);
		}

		inline Number<mpz_class> lcm(const Number<mpz_class>& b) const {
			mpz_class res;
			mpz_lcm(res.get_mpz_t(), mData.get_mpz_t(), b.mData.get_mpz_t());
			return Number(res);
		}

		/*

		NOTE: doesn't seem to be used anywhere

		inline mpz_class& gcd_assign(mpz_class& a, const mpz_class& b) {
		    a = carl::gcd(a,b);
			return a;
		}  */





		inline Number<mpz_class> pow(std::size_t exp) const {
			mpz_class res;
			mpz_pow_ui(res.get_mpz_t(), mData.get_mpz_t(), exp);
			return Number(res);
		}




		inline Number<mpz_class> mod(const Number<mpz_class>& m) const {
		    // TODO: In order to have the same result as division of native signed integer we have to
		    //       make it that complicated, as mpz_mod always returns positive integer. Maybe there is a better way.
			mpz_class res;
			mpz_mod(res.get_mpz_t(), this->abs().mData.get_mpz_t(), m.mData.get_mpz_t());
			return this->isNegative() ? Number(mpz_class(-res)) : Number(res);
		}

		inline Number<mpz_class> remainder(const Number<mpz_class>& m) const {
			return this->mod(m);
		}

		inline Number<mpz_class> quotient(const Number<mpz_class>& d) const
		{
		    // TODO: In order to have the same result as division of native signed integer we have to
		    //       make it that complicated, as mpz_div does round differently. Maybe there is a better way.
			mpz_class res;
			mpz_div(res.get_mpz_t(), this->abs().mData.get_mpz_t(), d.abs().mData.get_mpz_t());
			return this->isNegative() == d.isNegative() ? Number(res) : Number(mpz_class(-res));
		}
		
		//TODO: operators inside or outside the number-class?
		inline Number<mpz_class> operator/(const Number<mpz_class>& d) const
		{
			return this->quotient(d);
		}


/*		NOTE: doesn't seem to get used!

		inline void divide(const mpz_class& dividend, const mpz_class& divisor, mpz_class& quotient, mpz_class& remainder) {
			mpz_divmod(quotient.get_mpz_t(), remainder.get_mpz_t(), dividend.get_mpz_t(), divisor.get_mpz_t());
		} */


		/**
		 * Divide two integers.
		 * Asserts that the remainder is zero.
		 * @param a First argument.
		 * @param b Second argument.
		 * @return \f$ a / b \f$.
		 */
		inline Number<mpz_class> div(const Number<mpz_class>& b) const {
			assert(this->mod(b) == 0);
			return this->quotient(b);
		}


/*	NOTE: Doesn't seem to get used!

inline mpz_class& div_assign(mpz_class& a, const mpz_class& b) {
	a = carl::quotient(a, b);
    return a;
} */


		virtual std::string toString(bool _infix=true) const;


	};


	//template specializations of member-functions have to be outside the class:
	template<>
	inline sint Number<mpz_class>::toInt<sint>() {
	    assert(mData <= std::numeric_limits<sint>::max());
	    assert(mData >= std::numeric_limits<sint>::min());
	    return mpz_get_si(mData.get_mpz_t());
	}
	template<>
	inline uint Number<mpz_class>::toInt<uint>() {
	    assert(mData <= std::numeric_limits<uint>::max());
	    assert(mData >= std::numeric_limits<uint>::min());
	    return mpz_get_ui(mData.get_mpz_t());
	} 


}
