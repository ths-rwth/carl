/** 
 * @file:   numbers.h
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once
#include <cln/cln.h>
#include <gmpxx.h>

namespace carl
{
	template<typename RationalType>
	struct IntegralT
	{
		typedef int type;
	};

	template<>
	struct IntegralT<cln::cl_RA>
	{
		typedef cln::cl_I type;
	};
	
	template<>
	struct IntegralT<mpq_class>
	{
		typedef mpz_class type;
	};

	inline cln::cl_I getNum(const cln::cl_RA& rat)
	{
		return cln::numerator(rat);
	}
	
	inline cln::cl_I getDenom(const cln::cl_RA& rat)
	{
		return cln::denominator(rat);
	}
	
	inline cln::cl_RA pow(const cln::cl_RA& base, unsigned exp)
	{
		return cln::expt(base, exp);
	}
	
	
	inline const mpz_class& getNum(const mpq_class& rat)
	{
		return rat.get_num();
	}
	
	inline const mpz_class& getDenom(const mpq_class& rat)
	{
		return rat.get_den();
	}
	
	inline mpz_class gcd(const mpz_class& v1, const mpz_class& v2)
	{
		mpz_class res;
		mpz_gcd(res.get_mpz_t(), v1.get_mpz_t(), v2.get_mpz_t());
		return res;
	}
	
	inline mpz_class lcm(const mpz_class& v1, const mpz_class& v2)
	{
		mpz_class res;
		mpz_lcm(res.get_mpz_t(), v1.get_mpz_t(), v2.get_mpz_t());
		return res;
	}
	
}