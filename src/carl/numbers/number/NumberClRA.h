#pragma once

#include "Number.h"
#include "NumberClI.h"

namespace carl {

#ifdef USE_CLN_NUMBERS

	//TODO: where to put this?
	//NOTE: this can only be uncommented once adaption_cln/operations.h is not used anymore (defined twice)
	//static const cln::cl_RA ONE_DIVIDED_BY_10_TO_THE_POWER_OF_23 = cln::cl_RA(1)/cln::expt(cln::cl_RA(10), 23);
	//static const cln::cl_RA ONE_DIVIDED_BY_10_TO_THE_POWER_OF_52 = cln::cl_RA(1)/cln::expt(cln::cl_RA(10), 52);
	

	//template<>
	//class Number<cln::cl_I> ;

	template<>
	class Number<mpq_class> ;

	template<>
	class Number<mpz_class> ;
	

	template<>
	class Number<cln::cl_RA> : public BaseNumber<cln::cl_RA,Number<cln::cl_RA>> {
	private:
	cln::cl_RA scaleByPowerOfTwo(const cln::cl_RA& a, int exp) const; //auxiliary function 
	public:

		Number() = default;
		Number(const Number<cln::cl_RA>& n) = default;
		Number(Number<cln::cl_RA>&& n) = default;

		explicit Number(const cln::cl_RA& t): BaseNumber(t) {}
		explicit Number(cln::cl_RA&& t): BaseNumber(std::move(t)) {}
		Number(int n) : BaseNumber(n) {} // NOLINT
		Number(long long int n) { mData = cln::cl_RA(n); } // NOLINT
		Number(unsigned long long int n) { mData = cln::cl_RA(n); } // NOLINT


		//The following constructors can maybe be grouped together in a Rational-superclass	
		//TODO: explicit or not?
		explicit Number(double d) { mData = cln::rationalize(d); }
		explicit Number(float f) { mData = cln::rationalize(f); } 


		explicit Number(const std::string& s);

		Number(const Number<cln::cl_I>& a, const Number<cln::cl_I>& b);

	
		explicit Number(const Number<cln::cl_I>& n);


		explicit Number(const Number<mpq_class>& n);
		explicit Number(const Number<mpz_class>& n);




		Number<cln::cl_RA>& operator=(const Number<cln::cl_RA>& n) {
			this->mData = n.mData;
			return *this;
		}

		template<typename Other>
		Number<cln::cl_RA>& operator=(const Other& n) {
			this->mData = n;
			return *this;
		}

		Number<cln::cl_RA>& operator=(Number<cln::cl_RA>&& n) noexcept {
			this->mData = std::move(n.mData);
			return *this;
		}



		

		






		/**
		 * Get the bit size of the representation of a fraction.
		 * @param n A fraction.
		 * @return Bit size of n.
		 */
		inline std::size_t bitsize() const {
			return cln::integer_length(cln::numerator(mData)) + cln::integer_length(cln::denominator(mData));
		}

		/**
		 * Converts the given fraction to a double.
		 * @param n A fraction.
		 * @return Double.
		 */
		inline double toDouble() const {
			return cln::double_approx(mData);
		}



		template<typename Integer>
		inline Integer toInt() const;



		/**
		 * Get absolute value of a fraction.
		 * @return \f$|n|\f$.
		 */
		inline Number<cln::cl_RA> abs() const {
			return Number(cln::abs(mData));
		}

		/**
		 * Extract the numerator from a fraction.
		 * @return Numerator.
		 */
		inline Number<cln::cl_I> getNum() const {
			return Number<cln::cl_I>(cln::numerator(mData));
		}

		/**
		 * Extract the denominator from a fraction.
		 * @return Denominator.
		 */
		inline Number<cln::cl_I> getDenom() const {
			return Number<cln::cl_I>(cln::denominator(mData));
		}

		/**
		 * Round a fraction to next integer.
		 * @return The next integer.
		 */
		inline Number<cln::cl_I> round() const {
			return Number<cln::cl_I> (cln::round1(mData));
		}



		/**
		 * Round down a fraction.
		 * @return \f$\lfloor n \rfloor\f$.
		 */
		inline Number<cln::cl_I> floor() const {
			return Number<cln::cl_I> (cln::floor1(mData));
		}



		/**
		 * Round up a fraction.
		 * @return \f$\lceil n \rceil\f$.
		 */
		inline Number<cln::cl_I> ceil() const {
			return Number<cln::cl_I> (cln::ceiling1(mData));
		}





		/**
		 * Check if a fraction is integral.
		 * @return true.
		 */
		inline bool isInteger() const {
			return this->getDenom().isOne();
		}






		/**
		 * Calculate the greatest common divisor of two fractions.
		 * Asserts that the arguments are integral.
		 * @param b Second argument.
		 * @return Gcd of a and b.
		 */
		inline Number<cln::cl_RA> gcd(const Number<cln::cl_RA>& b) const {
			return Number(cln::gcd(cln::numerator(mData),cln::numerator(b.mData)) / cln::lcm(cln::denominator(mData),cln::denominator(b.mData)));
		}


		/**
		 * Calculate the least common multiple of two fractions.
		 * Asserts that the arguments are integral.
		 * @param b Second argument.
		 * @return Lcm of a and b.
		 */
		inline Number<cln::cl_RA> lcm(const Number<cln::cl_RA>& b) const {
		    assert( this->isInteger());
		    assert( b.isInteger() );
			return Number(cln::lcm(cln::numerator(mData),cln::numerator(b.mData)));
		}

		/**
		 * Calculate the power of some fraction to some positive integer.
		 * @param e Exponent.
		 * @return \f$n^e\f$
		 */
		inline Number<cln::cl_RA> pow(std::size_t e) const {
			return Number(cln::expt(mData, int(e)));
		}

		inline Number<cln::cl_RA> log() const {
			return Number(cln::rationalize(cln::realpart(cln::log(mData))));
		}

		//Note that with std::cos, std::sin the result is more precise than with cln::sin, cln::cos!!
		inline Number<cln::cl_RA> sin() const {
			return Number(std::sin(toDouble()));
		}

		inline Number<cln::cl_RA> cos() const {
			return Number(std::cos(toDouble()));
		}

		/**
		 * Calculate the square root of a fraction if possible.
		 *
		 * @param b A reference to the rational, in which the result is stored.
		 * @return true, if the number to calculate the square root for is a square;
		 *         false, otherwise.
		 */
		bool sqrt_exact(Number<cln::cl_RA>& b) const;

		Number<cln::cl_RA> sqrt() const;

		/**
		 * Calculate the square root of a fraction.
		 *
		 * If we are able to find a an \f$x\f$ such that \f$x\f$ is the exact root of \f$a\f$, \f$(x,x)\f$ is returned.
		 * If we can not find such a number (note that such a number might not even exist), \f$(x,y)\f$ is returned with \f$ x < \sqrt{a} < y \f$.
		 * Note that we try to find bounds that are very close to the actual square root. If a small representation is more important than a small interval, sqrt_fast should be used.
		 * @return Interval containing the square root of a.
		 */
		std::pair<Number<cln::cl_RA>, Number<cln::cl_RA>> sqrt_safe() const;

		/**
		 * Compute square root in a fast but less precise way.
		 * Use cln::sqrt() to obtain an approximation. If the result is rational, i.e. the result is exact, use this result.
		 * Otherwise use the nearest integers as bounds on the square root.
		 * @param a Some number.
		 * @return [x,x] if sqrt(a) = x is rational, otherwise [y,z] for y,z integer and y < sqrt(a) < z.
		 */
		std::pair<Number<cln::cl_RA>, Number<cln::cl_RA>> sqrt_fast() const;



		/**
		 * Divide two fractions.
		 * @param b Second argument.
		 * @return \f$ this / b \f$.
		 */
		inline Number<cln::cl_RA> div(const Number<cln::cl_RA>& b) const {
			return Number(mData / b.mData);
		}



		/**
		 * Divide two fractions.
		 * @param b Second argument.
		 * @return \f$ this / b \f$.
		 */
		inline Number<cln::cl_RA> quotient(const Number<cln::cl_RA>& b) const
		{
			return Number(mData / b.mData);
		}


		inline Number<cln::cl_RA> reciprocal()  const{
			return Number(cln::recip(mData));
		}

		std::string toString(bool _infix=true) const;



	};

		/**
		 * Convert a fraction to an integer.
		 * This method assert, that the given fraction is an integer, i.e. that the denominator is one.
		 * @param n A fraction.
		 * @return An integer.
		 */

		template<>
		inline Number<cln::cl_I> Number<cln::cl_RA>::toInt<Number<cln::cl_I>>() const {
			assert(isInteger());
			return Number<cln::cl_I>(getNum());
		}
	
		template<>
		inline sint Number<cln::cl_RA>::toInt<sint>() const {
			return toInt<Number<cln::cl_I>>().toInt<sint>();
		}
		template<>
		inline uint Number<cln::cl_RA>::toInt<uint>() const {
		    return toInt<Number<cln::cl_I>>().toInt<uint>();
		}


#endif

}
