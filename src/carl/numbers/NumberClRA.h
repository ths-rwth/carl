#pragma once

#include "Number.h"



namespace carl {

#ifdef USE_CLN_NUMBERS

	template<>
	class Number<cln::cl_RA> : public BaseNumber<cln::cl_RA> {

		Number(): BaseNumber() {}
		explicit Number(const cln::cl_RA& t): BaseNumber(t) {}
		explicit Number(cln::cl_RA&& t): BaseNumber(t) {}
		Number(const Number<cln::cl_RA>& n): BaseNumber(n) {}
		Number(Number<cln::cl_RA>&& n) noexcept : BaseNumber(n) {}
		Number(long long int n) : BaseNumber(n) {}
		Number(unsigned long long int n): BaseNumber(n) {}


		//The following constructors can maybe be grouped together in a Rational-superclass	
		//TODO: explicit or not?
		Number(float f) { mData = cln::cl_RA(f); }
		Number(double d) { mData = cln::cl_RA(d); }

		Number(const std::string& s);

		//constructs a/b:
		Number(const Number<cln::cl_I>& a,const Number<cln::cl_I>& b) { mData = cln::cl_RA(a.getNumber(),b.getNumber()); }

	
		Number(const Number<cln::cl_I>& n) { mData = cln::cl_RA(n.getNumber()); }
		Number(const cln::cl_I& n) { mData = cln::cl_RA(n); }

		//TODO: check if this works or if there is a better possibility. Otherwise maybe retrieve "pieces" that fit into the data type and add them together again
		Number(const Number<mpq_class>& n) : Number(n.toString()) {} 
		Number(const Number<mpz_class>& n) : Number(n.toString()) {} 


		

		
		//could probably be changed such that it's the same as in all other classes
		inline bool isZero() {
			return zerop(mData);
		}


		//these 3 methods are the same as for mpq, mpz
		inline bool isOne() {
			return mData  == carl::constant_one<cln::cl_RA>().get();
		}


		inline bool isPositive() {
			return mData > carl::constant_zero<cln::cl_RA>().get();
		}


		inline bool isNegative() {
			return mData < carl::constant_zero<cln::cl_RA>().get();
		}

		/**
		 * Extract the numerator from a fraction.
		 * @return Numerator.
		 */
		inline Number<cln::cl_I> getNum() {
			return Number(cln::numerator(mData));
		}

		/**
		 * Extract the denominator from a fraction.
		 * @return Denominator.
		 */
		inline Number<cln::cl_I> getDenom() {
			return Number(cln::denominator(mData));
		}

		/**
		 * Check if a fraction is integral.
		 * @return true.
		 */
		inline bool isInteger() {
			return this->getDenom().isOne();
		}


		/**
		 * Get the bit size of the representation of a fraction.
		 * @param n A fraction.
		 * @return Bit size of n.
		 */
		inline std::size_t bitsize() {
			return cln::integer_length(cln::numerator(mData)) + cln::integer_length(cln::denominator(mData));
		}

		/**
		 * Converts the given fraction to a double.
		 * @param n A fraction.
		 * @return Double.
		 */
		inline double toDouble() {
			return cln::double_approx(mData);
		}



		template<typename Integer>
		inline Integer toInt();



		/**
		 * Convert a fraction to an integer.
		 * This method assert, that the given fraction is an integer, i.e. that the denominator is one.
		 * @param n A fraction.
		 * @return An integer.
		 */
		template<>
		inline Number<cln::cl_I> toInt<cln::cl_I>() {
			assert(isInteger());
			return getNum();
		}
		//TODO: is this correct?!
		template<>
		inline sint toInt<sint>() {
			return toInt<sint>(toInt<cln::cl_I>(n));
		}
		template<>
		inline uint toInt<uint>() {
		    return toInt<uint>(toInt<cln::cl_I>(n));
		}

		//TODO: where to put this?
		static const cln::cl_RA ONE_DIVIDED_BY_10_TO_THE_POWER_OF_23 = cln::cl_RA(1)/cln::expt(cln::cl_RA(10), 23);
		static const cln::cl_RA ONE_DIVIDED_BY_10_TO_THE_POWER_OF_52 = cln::cl_RA(1)/cln::expt(cln::cl_RA(10), 52);

		
		//TODO: Rationalize as constructors!!
		template<>
		cln::cl_RA rationalize<cln::cl_RA>(double n);

		template<>
		cln::cl_RA rationalize<cln::cl_RA>(float n);

		template<>
		inline cln::cl_RA rationalize<cln::cl_RA>(int n) {
		    return cln::cl_RA(n);
		}

		template<>
		inline cln::cl_RA rationalize<cln::cl_RA>(uint n) {
			return cln::cl_RA(n);
		}

		template<>
		inline cln::cl_RA rationalize<cln::cl_RA>(sint n) {
			return cln::cl_RA(n);
		}

		template<>
		cln::cl_RA rationalize<cln::cl_RA>(const std::string& inputstring);



		/**
		 * Get absolute value of a fraction.
		 * @return \f$|n|\f$.
		 */
		inline Number<cln::cl_RA> abs() {
			return Number(cln::abs(mData));
		}

		/**
		 * Round a fraction to next integer.
		 * @return The next integer.
		 */
		inline Number<cln::cl_I> round() {
			return Number(cln::round1(mData));
		}



		/**
		 * Round down a fraction.
		 * @return \f$\lfloor n \rfloor\f$.
		 */
		inline Number<cln::cl_I> floor() {
			return Number(cln::floor1(mData));
		}



		/**
		 * Round up a fraction.
		 * @return \f$\lceil n \rceil\f$.
		 */
		inline Number<cln::cl_I> ceil() {
			return Number(cln::ceiling1(mData));
		}





		/**
		 * Calculate the greatest common divisor of two fractions.
		 * Asserts that the arguments are integral.
		 * @param b Second argument.
		 * @return Gcd of a and b.
		 */
		inline Number<cln::cl_RA> gcd(const Number<cln::cl_RA>& b) {
			return Number(cln::gcd(cln::numerator(mData),cln::numerator(b.mData)) / cln::lcm(cln::denominator(mData),cln::denominator(b.mData)));
		}


		/**
		 * Calculate the least common multiple of two fractions.
		 * Asserts that the arguments are integral.
		 * @param b Second argument.
		 * @return Lcm of a and b.
		 */
		inline Number<cln::cl_RA> lcm(const Number<cln::cl_RA>& b) {
		    assert( this->isInteger());
		    assert( b.isInteger() );
			return cln::lcm(cln::numerator(mData),cln::numerator(b.mData));
		}

		/**
		 * Calculate the power of some fraction to some positive integer.
		 * @param e Exponent.
		 * @return \f$n^e\f$
		 */
		template<>
		inline Number<cln::cl_RA> pow(std::size_t e) {
			return Number(cln::expt(mData, int(e)));
		}

		inline Number<cln::cl_RA> log() {
			return Number(cln::rationalize(cln::realpart(cln::log(mData))));
		}

		inline Number<cln::cl_RA> sin() {
			return Number(cln::rationalize(cln::sin(mData)));
		}

		inline Number<cln::cl_RA> cos() {
			return Number(cln::rationalize(cln::cos(mData)));
		}

		/**
		 * Calculate the square root of a fraction if possible.
		 *
		 * @param b A reference to the rational, in which the result is stored.
		 * @return true, if the number to calculate the square root for is a square;
		 *         false, otherwise.
		 */
		bool sqrt_exact(Number<cln::cl_RA>& b);

		Number<cln::cl_RA> sqrt();

		/**
		 * Calculate the square root of a fraction.
		 *
		 * If we are able to find a an \f$x\f$ such that \f$x\f$ is the exact root of \f$a\f$, \f$(x,x)\f$ is returned.
		 * If we can not find such a number (note that such a number might not even exist), \f$(x,y)\f$ is returned with \f$ x < \sqrt{a} < y \f$.
		 * Note that we try to find bounds that are very close to the actual square root. If a small representation is more important than a small interval, sqrt_fast should be used.
		 * @return Interval containing the square root of a.
		 */
		std::pair<Number<cln::cl_RA>, Number<cln::cl_RA>> sqrt_safe();

		/**
		 * Compute square root in a fast but less precise way.
		 * Use cln::sqrt() to obtain an approximation. If the result is rational, i.e. the result is exact, use this result.
		 * Otherwise use the nearest integers as bounds on the square root.
		 * @param a Some number.
		 * @return [x,x] if sqrt(a) = x is rational, otherwise [y,z] for y,z integer and y < sqrt(a) < z.
		 */
		std::pair<Number<cln::cl_RA>, Number<cln::cl_RA>> sqrt_fast();



		/**
		 * Divide two fractions.
		 * @param b Second argument.
		 * @return \f$ this / b \f$.
		 */
		inline Number<cln::cl_RA> div(const Number<cln::cl_RA>& b) {
			return Number(mData / b.mData);
		}



		/**
		 * Divide two fractions.
		 * @param b Second argument.
		 * @return \f$ this / b \f$.
		 */
		inline Number<cln::cl_RA> quotient(const Number<cln::cl_RA>& b)
		{
			return Number(mData / b.mData);
		}


		inline Number<cln::cl_RA> reciprocal() {
			return Number(cln::recip(mData));
		}

		std::string toString(bool _infix=true) const;



	};


#endif

}

