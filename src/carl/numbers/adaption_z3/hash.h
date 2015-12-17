/** 
 * @file    adaption_gmpxx/hash.h
 * @ingroup gmpxx
 * @author  Sebastian Junges
 * @author  Florian Corzilius
 *
 */
#pragma once
#include <functional>

#include "z3_include.h"

namespace std
{
// This is actually gmp
//template<>
//struct hash<mpq_t>
//{
//	size_t operator()(const mpq_t& gmp_rational) const
//	{
//		return mpz_get_ui(mpq_numref(gmp_rational)) ^ mpz_get_ui(mpq_denref(gmp_rational));
//	}
//};

template<> 
struct hash<mpz>
{
	size_t operator()(const mpz& z) const
	{
		return mpz_manager<false>::hash(z);
	}
};
	
template<>
struct hash<mpq>
{
	size_t operator()(const mpq& q) const
	{
		return mpq_manager<false>::hash(q);
	}
};

}
