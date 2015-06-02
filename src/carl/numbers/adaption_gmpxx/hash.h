/** 
 * @file    adaption_gmpxx/hash.h
 * @ingroup gmpxx
 * @author  Sebastian Junges
 * @author  Florian Corzilius
 *
 */

#pragma once
#include <functional>
#include <cstddef>
#ifdef __WIN
	#pragma warning(push, 0)
	#include <mpirxx.h>
	#pragma warning(pop)
#else
	#include <gmpxx.h>
#endif

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
struct hash<mpz_class>
{
	size_t operator()(const mpz_class& z) const
	{
		return z.get_ui();
	}
};
	
template<>
struct hash<mpq_class>
{
	size_t operator()(const mpq_class& q) const
	{
		std::hash<mpz_class> H;
		return H(q.get_num()) ^ H(q.get_den());
	}
};

}
