/** 
 * @file:   numbers.h
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once
#include <climits>
#include <cln/cln.h>
#include <gmpxx.h>
#include <functional>
#include <array>
#include <map>

#include "typetraits.h"
#include "operations.h"

namespace std
{

template<>
struct hash<cln::cl_RA>
{
	size_t operator()(const cln::cl_RA& cln_rational) const
	{
		return cln::equal_hashcode(cln_rational);
	}
};

template<>
struct hash<mpq_t>
{
	size_t operator()(const mpq_t& gmp_rational) const
	{
		return mpz_get_ui(mpq_numref(gmp_rational)) ^ mpz_get_ui(mpq_denref(gmp_rational));
	}
};
} // namespace std
