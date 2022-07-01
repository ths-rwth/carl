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

TRAIT_TRUE(is_subset_of_integers_type, signed char, );
TRAIT_TRUE(is_subset_of_integers_type, short int, );
TRAIT_TRUE(is_subset_of_integers_type, int, );
TRAIT_TRUE(is_subset_of_integers_type, long int, );
TRAIT_TRUE(is_subset_of_integers_type, long long int, );

TRAIT_TRUE(is_subset_of_integers_type, unsigned char, );
TRAIT_TRUE(is_subset_of_integers_type, unsigned short int, );
TRAIT_TRUE(is_subset_of_integers_type, unsigned int, );
TRAIT_TRUE(is_subset_of_integers_type, unsigned long int, );
TRAIT_TRUE(is_subset_of_integers_type, unsigned long long int, );


TRAIT_TYPE(IntegralType, float, sint, );
TRAIT_TYPE(IntegralType, double, sint, );
TRAIT_TYPE(IntegralType, long double, sint, );

}
