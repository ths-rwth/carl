/** 
 * @file    adaption_gmpxx/hash.h
 * @ingroup gmpxx
 * @author  Sebastian Junges
 * @author  Florian Corzilius
 *
 */
#pragma once
#include <functional>

#include "include.h"

namespace std
{

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
