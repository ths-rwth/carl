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

#include "../../util/platform.h"
#include <cstddef>
CLANG_WARNING_DISABLE("-Wmismatched-tags")
CLANG_WARNING_DISABLE("-Wsign-conversion")
#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
#endif
#include <gmpxx.h>
CLANG_WARNING_RESET

#include "../typetraits.h"

namespace carl {
	
TRAIT_TRUE(is_integer, mpz_class, gmpxx);
TRAIT_TRUE(is_rational, mpq_class, gmpxx);

TRAIT_TYPE(IntegralType, mpq_class, mpz_class, gmpxx);
TRAIT_TYPE(IntegralType, mpz_class, mpz_class, gmpxx);

}
