/** 
 * @file   adaption_cln/typetraits.h
 * @ingroup cln
 * @ingroup typetraits
 * @author Sebastian Junges
 * @author Gereon Kremer
 */

#pragma once

#ifndef INCLUDED_FROM_NUMBERS_H
static_assert(false, "This file may only be included indirectly by numbers.h");
#endif

#include "include.h"
#include "../typetraits.h"

namespace carl {

TRAIT_TRUE(is_integer, cln::cl_I, cln);
TRAIT_TRUE(is_rational, cln::cl_RA, cln);

TRAIT_TYPE(IntegralType, cln::cl_I, cln::cl_I, cln);
TRAIT_TYPE(IntegralType, cln::cl_RA, cln::cl_I, cln);

}
