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
	
TRAIT_TRUE(is_integer_type, mpz_class, gmpxx);
TRAIT_TRUE(is_rational_type, mpq_class, gmpxx);

TRAIT_TYPE(IntegralType, mpq_class, mpz_class, gmpxx);
TRAIT_TYPE(IntegralType, mpz_class, mpz_class, gmpxx);

}
