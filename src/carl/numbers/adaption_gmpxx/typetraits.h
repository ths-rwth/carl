/** 
 * @file   adaption_gmpxx/typetraits.h
 * @ingroup typetraits
 * @ingroup gmpxx
 * @author Sebastian Junges
 * @author Gereon Kremer
 *
 */

#pragma once

#include "../../util/platform.h"
CLANG_WARNING_DISABLE("-Wsign-conversion")
#include <gmpxx.h>
CLANG_WARNING_RESET

#include "../typetraits.h"

namespace carl {
	
TRAIT_TRUE(is_integer, mpz_class, gmpxx);
TRAIT_TRUE(is_rational, mpq_class, gmpxx);

TRAIT_TYPE(IntegralType, mpq_class, mpz_class, gmpxx);
TRAIT_TYPE(IntegralType, mpz_class, mpz_class, gmpxx);

}

namespace std {

TRAIT_TRUE(is_integral, mpz_class, gmpxx);
TRAIT_FALSE(is_fundamental, mpz_class, gmpxx);
TRAIT_FALSE(is_fundamental, mpq_class, gmpxx);

}