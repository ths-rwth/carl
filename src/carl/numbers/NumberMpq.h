#pragma once

#include "Number.h"



namespace carl {

	template<>
	class Number<mpq_class> : public BaseNumber<mpq_class> {
	public:

		Number(): BaseNumber() {}
		explicit Number(const mpq_class& t): BaseNumber(t) {}
		explicit Number(mpq_class&& t): BaseNumber(t) {}
		Number(const Number<mpq_class>& n): BaseNumber(n) {}
		Number(Number<mpq_class>&& n) noexcept : BaseNumber(n) {}
		Number(long long int n) : BaseNumber(n) {}
		Number(unsigned long long int n): BaseNumber(n) {}

		
		//The following constructors can maybe be grouped together in a Rational-superclass	
		//TODO: explicit or not?
		Number(float f) { mData = mpq_class(f); }
		Number(double d) { mData = mpq_class(d); }

		Number(const std::string& s);

		//constructs a/b:
		Number(const Number<mpz_class>& a,const Number<mpz_class>& b) { mData = mpq_class(a.getNumber(),b.getNumber()); }

	
		Number(const Number<mpz_class>& n) { mData = mpq_class(n.getNumber()); }
		Number(const mpz_class& n) { mData = mpq_class(n); }

		std::string toString(bool _infix=true) const;

		//TODO: check if this works or if there is a better possibility. Otherwise maybe retrieve "pieces" that fit into the data type and add them together again

#ifdef USE_CLN_NUMBERS
		Number(const Number<cln::cl_RA>& n) : Number(n.toString()) {} 
		Number(const Number<cln::cl_I>& n) : Number(n.toString()) {} 
#endif
		
		

		//NOTE: isZero, isOne are exactly the same as for mpz_class!!
		inline bool isZero() {
			return constant_zero<mpz_class>::get() == mData;
		}


		inline bool isOne() {
			return constant_one<mpz_class>::get() == mData;
		}


		//these two are the same as for mpz_class, just template-dependent
		inline bool isPositive() {
			return mData > carl::constant_zero<mpq_class>().get();
		}

		inline bool isNegative() {
			return mData < carl::constant_zero<mpq_class>().get();
		}




		inline Number<mpz_class> getNum() {
			return Number<mpz_class>(mData.get_num());
		}

		inline Number<mpz_class> getDenom() {
			return Number<mpz_class>(mData.get_den());
		}


		inline bool isInteger() {
			 return 0 != mpz_divisible_p(mData.get_num_mpz_t(), mData.get_den_mpz_t());
		}



		/**
		 * Get the bit size of the representation of a fraction.
		 * @param n A fraction.
		 * @return Bit size of n.
		 */
		inline std::size_t bitsize() {
			return mpz_sizeinbase(mData.get_num().__get_mp(),2) + mpz_sizeinbase(mData.get_den().__get_mp(),2);
		}

		/**
		 * Conversion functions
		 *
		 * The following function convert types to other types.
		 */

		//this is the same as for mpz
		inline double toDouble() {
			return mData.get_d();
		}



		template<typename Integer>
		inline Integer toInt();



		/**
		 * Basic Operators
		 *
		 * The following functions implement simple operations on the given numbers.
		 */


		inline Number<mpq_class> abs() {
			mpq_class res;
			mpq_abs(res.get_mpq_t(), mData.get_mpq_t());
			return Number(res);
		}

		inline Number<mpz_class> round() {
			if (Number(mpz_class(mData.get_num_mpz_t())).isZero()) return Number<mpz_class>(carl::constant_zero<mpz_class>::get());
			mpz_class res;
			mpz_class rem;
			mpz_fdiv_qr(res.get_mpz_t(), rem.get_mpz_t(), mData.get_num_mpz_t(), mData.get_den_mpz_t());
			rem *= 2;
			if (rem >= mData.get_den()) ++res;
			return Number<mpz_class>(res);
		}

		inline Number<mpz_class> floor() {
			if (Number(mpz_class(mData.get_num_mpz_t())).isZero()) return Number<mpz_class>(carl::constant_zero<mpz_class>::get());
			mpz_class res;
			mpz_fdiv_q(res.get_mpz_t(), mData.get_num_mpz_t(), mData.get_den_mpz_t());
			return Number<mpz_class>(res);
		}


		inline Number<mpz_class> ceil() {
			if (Number(mpz_class(mData.get_num_mpz_t())).isZero()) return Number<mpz_class>(carl::constant_zero<mpz_class>::get());
			mpz_class res;
			mpz_cdiv_q(res.get_mpz_t(), mData.get_num_mpz_t(), mData.get_den_mpz_t());
			return Number<mpz_class>(res);
		}



		inline Number<mpq_class> gcd(const Number<mpq_class>& b) {
		    	mpz_class resNum;
			mpz_gcd(resNum.get_mpz_t(), mData.get_num().get_mpz_t(), b.mData.get_num().get_mpz_t());
			mpz_class resDen;
			mpz_lcm(resDen.get_mpz_t(), mData.get_den().get_mpz_t(), b.mData.get_den().get_mpz_t());
			mpq_class resqNum;
			mpq_set_z(resqNum.get_mpq_t(), resNum.get_mpz_t());
			mpq_class resqDen;
			mpq_set_z(resqDen.get_mpq_t(), resDen.get_mpz_t());
			mpq_class res;
			mpq_div(res.get_mpq_t(), resqNum.get_mpq_t(), resqDen.get_mpq_t());
			return Number(res);
		}



		inline Number<mpq_class> lcm(const Number<mpq_class>& b) {
		    	mpz_class resNum;
			mpz_lcm(resNum.get_mpz_t(), mData.get_num().get_mpz_t(), b.mData.get_num().get_mpz_t());
			mpz_class resDen;
			mpz_gcd(resDen.get_mpz_t(), mData.get_den().get_mpz_t(), b.mData.get_den().get_mpz_t());
			mpq_class resqNum;
			mpq_set_z(resqNum.get_mpq_t(), resNum.get_mpz_t());
			mpq_class resqDen;
			mpq_set_z(resqDen.get_mpq_t(), resDen.get_mpz_t());
			mpq_class res;
			mpq_div(res.get_mpq_t(), resqNum.get_mpq_t(), resqDen.get_mpq_t());
			return Number(res);
		}

		inline Number<mpq_class> log() {
			return Number(std::log(this->toDouble()));
		}

		inline Number<mpq_class> sin() {
			return Number(std::sin(this->toDouble()));
		}

		inline Number<mpq_class> cos() {
			return Number(std::cos(this->toDouble()));
		}


		inline Number<mpq_class> pow(std::size_t e) {
			mpz_class den = mData.get_den();
			mpz_class powDen;
			mpz_pow_ui(powDen.get_mpz_t(), den.get_mpz_t(), e);
			mpz_class num = mData.get_num();
			mpz_class powNum;
			mpz_pow_ui(powNum.get_mpz_t(), num.get_mpz_t(), e);
			mpq_class resNum;
			mpq_set_z(resNum.get_mpq_t(), powNum.get_mpz_t());
			mpq_class resDen;
			mpq_set_z(resDen.get_mpq_t(), powDen.get_mpz_t());
			mpq_class res;
			mpq_div(res.get_mpq_t(), resNum.get_mpq_t(), resDen.get_mpq_t());
			return Number(res);
		}

		/**
		 * Calculate the square root of a fraction if possible.
		 *
		 * @param b A reference to the rational, in which the result is stored.
		 * @return true, if the number to calculate the square root for is a square;
		 *         false, otherwise.
		 */
		bool sqrt_exact(Number<mpq_class>& b);

		Number<mpq_class> sqrt();

		std::pair<Number<mpq_class>,Number<mpq_class>> sqrt_safe();

		/**
		 * Compute square root in a fast but less precise way.
		 * Use cln::sqrt() to obtain an approximation. If the result is rational, i.e. the result is exact, use this result.
		 * Otherwise use the nearest integers as bounds on the square root.
		 * @param a Some number.
		 * @return [x,x] if sqrt(a) = x is rational, otherwise [y,z] for y,z integer and y < sqrt(a) < z.
		 */
		std::pair<Number<mpq_class>,Number<mpq_class>> sqrt_fast();



		inline Number<mpq_class> quotient(const Number<mpq_class>& d)
		{
			mpq_class res;
			mpq_div(res.get_mpq_t(), mData.get_mpq_t(), d.mData.get_mpq_t());
			return Number(res);
		}



		//this and div is the same as for mpz!!
		inline Number<mpq_class> operator/(const Number<mpq_class>& d)
		{
			return this->quotient(d);
		}


		inline Number<mpq_class> div(const Number<mpq_class>& b) {
			return this->quotient(b);
		}




		inline Number<mpq_class> reciprocal() {
			mpq_class res;
			mpq_inv(res.get_mpq_t(), mData.get_mpq_t());
			return Number(res);
		}

		inline Number<mpq_class> operator *(const Number<mpq_class>& rhs)
		{
			mpq_class res;
			mpq_mul(res.get_mpq_t(), mData.get_mpq_t(), rhs.mData.get_mpq_t());
			return Number(res);
		}


	};

	
	//template specializations of template member-functions of Number<mpq_class>

	//TODO: is this one necessary or should it be done in a constructor?
	template<>
	inline Number<mpz_class> Number<mpq_class>::toInt<Number<mpz_class>>() {
		assert(this->isInteger());
		return this->getNum();
	}


	template<>
	inline sint Number<mpq_class>::toInt<sint>() {
	    return this->toInt<Number<mpz_class>>().toInt<sint>();
	}
	template<>
	inline uint Number<mpq_class>::toInt<uint>() {
		return this->toInt<Number<mpz_class>>().toInt<uint>();
	}


}

