/** 
 * @file    hash.h
 * @ingroup gmpxx
 * @author  Sebastian Junges
 * @author  Florian Corzilius
 *
 */

#pragma once
#include <functional>
#include <gmpxx.h>

namespace std
{
template<>
struct hash<mpq_t>
{
	size_t operator()(const mpq_t& gmp_rational) const
	{
		return mpz_get_ui(mpq_numref(gmp_rational)) ^ mpz_get_ui(mpq_denref(gmp_rational));
	}
};
}