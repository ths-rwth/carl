/** 
 * @file   adaption_gmpxx/typetraits.h
 * @ingroup typetraits
 * @ingroup gmpxx
 * @author Sebastian Junges
 * @author Gereon Kremer
 *
 */
#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include "../typetraits.h"
#include "include.h"

namespace carl {
	
TRAIT_TRUE(is_integer, mpz, z3);
TRAIT_TRUE(is_rational, mpq, z3);
TRAIT_TRUE(is_rational, rational, z3);

TRAIT_TYPE(IntegralType, mpq, mpz, z3);
TRAIT_TYPE(IntegralType, mpz, mpz, z3);
}
