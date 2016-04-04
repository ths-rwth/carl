/**
 * @file adaption_native/typetraits.h
 * @ingroup typetraits
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include "../typetraits.h"

namespace carl {

TRAIT_TRUE(is_subset_of_integers, signed char, );
TRAIT_TRUE(is_subset_of_integers, short int, );
TRAIT_TRUE(is_subset_of_integers, int, );
TRAIT_TRUE(is_subset_of_integers, long int, );
TRAIT_TRUE(is_subset_of_integers, long long int, );

TRAIT_TRUE(is_subset_of_integers, unsigned char, );
TRAIT_TRUE(is_subset_of_integers, unsigned short int, );
TRAIT_TRUE(is_subset_of_integers, unsigned int, );
TRAIT_TRUE(is_subset_of_integers, unsigned long int, );
TRAIT_TRUE(is_subset_of_integers, unsigned long long int, );


TRAIT_TYPE(IntegralType, float, sint, );
TRAIT_TYPE(IntegralType, double, sint, );
TRAIT_TYPE(IntegralType, long double, sint, );

}
