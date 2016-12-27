#pragma once


#include "Number.h"
#include "NumberMpz.h"



namespace carl {




#ifdef USE_CLN_NUMBERS
	template<>
	class Number<cln::cl_I> ;

	template<>
	class Number<cln::cl_RA> ;
#endif



	template<>
	class Number<mpz_class> ;

	template<>
	class Number<mpq_class> : public BaseNumber<mpq_class,Number<mpq_class>> {
	public:

		Number(): BaseNumber() {}
		explicit Number(const mpq_class& t): BaseNumber(t) {}
		explicit Number(mpq_class&& t): BaseNumber(t) {}
		Number(const Number<mpq_class>& n): BaseNumber(n) {}
		Number(Number<mpq_class>&& n) noexcept : BaseNumber(n) {}
		Number(int i) : BaseNumber(i) {}
		Number(long long int n) : BaseNumber(n) {}
		Number(unsigned long long int n): BaseNumber(n) {}

		
		//The following constructors can maybe be grouped together in a Rational-superclass	
		//TODO: explicit or not?
		Number(float f) { mData = mpq_class(f); }
		Number(double d) { mData = mpq_class(d); }

		explicit Number(const std::string& s);

		//constructs a/b:
		Number(const Number<mpz_class>& a,const Number<mpz_class>& b);

	
		Number(const Number<mpz_class>& n);
		Number(const mpz_class& n);

#ifdef USE_CLN_NUMBERS
		Number(const Number<cln::cl_RA>& n);
		Number(const Number<cln::cl_I>& n);
#endif

		std::string toString(bool _infix=true) const;





		Number<mpq_class>& operator=(const Number<mpq_class>& n) {
			this->mData = n.mData;
			return *this;
		}

		template<typename Other>
		Number<mpq_class>& operator=(const Other& n) {
			this->mData = n;
			return *this;
		}

		Number<mpq_class>& operator=(Number<mpq_class>&& n) noexcept {
			this->mData = std::move(n.mData);
			return *this;
		}




		inline Number<mpz_class> getNum() const {
			return Number<mpz_class>(mData.get_num());
		}

		inline Number<mpz_class> getDenom() const {
			return Number<mpz_class>(mData.get_den());
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


		inline Number<mpq_class> abs() const {
			mpq_class res;
			mpq_abs(res.get_mpq_t(), mData.get_mpq_t());
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




		inline Number<mpq_class> div(const Number<mpq_class>& b) {
			return this->quotient(b);
		}




		inline Number<mpq_class> reciprocal() {
			mpq_class res;
			mpq_inv(res.get_mpq_t(), mData.get_mpq_t());
			return Number(res);
		}



	};

	//template specializations of template member-functions of Number<mpq_class>

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

