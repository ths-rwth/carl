#pragma once

namespace carl {

	template<typename T>
	class BaseNumber {
	protected:
		T mData;
	public:

		//TODO: which constructors explicit?
		//TODO: check if these constructors actually work for all possible instantiations
		//TODO: possibly implement setValue(T) as well
		BaseNumber(): mData(carl::constant_zero<T>::get()) {}
		explicit BaseNumber(const T& t): mData(t) {}
		explicit BaseNumber(T&& t): mData(std::move(t)) {}
		BaseNumber(const BaseNumber<T>& n): mData(n.mData) {}
		BaseNumber(BaseNumber<T>&& n) noexcept : mData(std::move(n.mData)) {}
		BaseNumber(long long int n) : mData(n) {}
		BaseNumber(unsigned long long int n): mData(n) {}
		BaseNumber(const std::string& s) : mData(s) {}

		const T& getNumber() {
			return mData;
		};

		BaseNumber<T>& operator=(const BaseNumber<T>& n) {
			mData = n.mData;
			return *this;
		}

		template<typename Other>
		BaseNumber<T>& operator=(const Other& n) {
			mData = n;
			return *this;
		}

		BaseNumber<T>& operator=(BaseNumber<T>&& n) noexcept {
			mData = std::move(n.mData);
			return *this;
		}

		operator T&() {
			return mData;
		}
		operator const T&() const {
			return mData;
		}
	};

	template<typename T>
	class Number : BaseNumber<T> {};



	template<>
	class Number<mpz_class> : BaseNumber<mpz_class> {
	public:
	

		Number(): BaseNumber() {}
		explicit Number(const mpz_class& t): BaseNumber(t) {}
		explicit Number(mpz_class&& t): BaseNumber(t) {}
		Number(const Number<mpz_class>& n): BaseNumber(n) {}
		Number(Number<mpz_class>&& n) noexcept : BaseNumber(n) {}


	
		
		inline bool isZero() const {
			return constant_zero<mpz_class>::get() == mData;
		}

		inline bool isOne() const {
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


		std::string toString(bool _infix=true);


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




	template<>
	class Number<mpq_class> : BaseNumber<mpq_class> {
	public:

		Number(): BaseNumber() {}
		explicit Number(const mpq_class& t): BaseNumber(t) {}
		explicit Number(mpq_class&& t): BaseNumber(t) {}
		Number(const Number<mpq_class>& n): BaseNumber(n) {}
		Number(Number<mpq_class>&& n) noexcept : BaseNumber(n) {}

		//TODO: explicit or not?
		Number(float f) { mData = mpq_class(f); }
		Number(double d) { mData = mpq_class(d); }

		Number(const std::string& s);

		//constructs a/b:
		//TODO: this...
		Number(const Number<mpz_class>& a,const Number<mpz_class>& b) {}

		//TODO: conversion constructors
		

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

		std::string toString(bool _infix=true);

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







	template<typename T>
	Number<T> operator+(const Number<T>& lhs, const Number<T>& rhs) {
		return T(T(lhs) + T(rhs));
	}
	template<typename T>
	Number<T> operator+=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) += T(rhs);
	}
	template<typename T>
	Number<T> operator-(const Number<T>& lhs, const Number<T>& rhs) {
		return T(T(lhs) - T(rhs));
	}
	template<typename T>
	Number<T> operator-=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) -= T(rhs);
	}
	template<typename T>
	Number<T> operator*(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) * T(rhs);
	}
	template<typename T>
	Number<T> operator*=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) *= T(rhs);
	}
	template<typename T>
	Number<T> operator/(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) / T(rhs);
	}
	template<typename T>
	Number<T> operator/=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) /= T(rhs);
	}

	template<typename T>
	bool operator==(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) == T(rhs);
	}
	template<typename T>
	bool operator!=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) != T(rhs);
	}
	template<typename T>
	bool operator<(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) < T(rhs);
	}
	template<typename T>
	bool operator<=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) <= T(rhs);
	}
	template<typename T>
	bool operator>=(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) >= T(rhs);
	}
	template<typename T>
	bool operator>(const Number<T>& lhs, const Number<T>& rhs) {
		return T(lhs) > T(rhs);
	}

	template<typename T>
	std::ostream& operator <<(std::ostream& os, const Number<T>& n) {
		return os << T(n);
	}




		
}

/* template<typename T>
class Number {
public:
        Number(const T&);
        Number(long long int)
        Number(unsigned long long int)
        Number(std::string)
        const T& getNum();
};

template<>
class Number<mpq_class>: public PlusMixin<mpq_class, Number> {
       
        explicit Number(const Number<mpz_class>&);
};



mpz_class
mpq_class
cln::cl_I
cln::cl_RA

int, unsigned, float, double
mpfr

https://gmplib.org/manual/
http://www.ginac.de/CLN/cln.html

*/
