/** 
 * @file   adaption_cln/typetraits.h
 * @ingroup cln
 * @ingroup typetraits
 * @author Sebastian Junges
 * @author Gereon Kremer
 */

#pragma once

#include "../../util/platform.h"
CLANG_WARNING_DISABLE("-Wmismatched-tags")
CLANG_WARNING_DISABLE("-Wsign-conversion")
#include <cln/cln.h>
CLANG_WARNING_RESET

#include "../typetraits.h"

namespace carl {

TRAIT_TRUE(is_integer, cln::cl_I, cln);
TRAIT_TRUE(is_rational, cln::cl_RA, cln);
TRAIT_TYPE(IntegralType, cln::cl_I, cln::cl_I, cln);
TRAIT_TYPE(IntegralType, cln::cl_RA, cln::cl_I, cln);

}

namespace std {

//TRAIT_TRUE(is_integral, cln::cl_I, "cln");

}