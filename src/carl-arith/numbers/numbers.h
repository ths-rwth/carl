/**
 * @file:   numbers.h
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once

#define INCLUDED_FROM_NUMBERS_H true

#include <cstdint>
#include <type_traits>

namespace carl {
	using uint = std::uint64_t;
	using sint = std::int64_t;
	
	static_assert(std::is_arithmetic<uint>::value);
	static_assert(std::is_arithmetic<sint>::value);
}

#include <carl-common/config.h>
#include "constants.h"
#include "typetraits.h"
#include "operations_predeclarations.h"
#include "operations_generic.h"

#include "adaption_native/operations.h"
#include "adaption_native/typetraits.h"

#include "adaption_gmpxx/hash.h"
#include "adaption_gmpxx/operations.h"
#include "adaption_gmpxx/typetraits.h"


#ifdef USE_CLN_NUMBERS
#include "adaption_cln/include.h"
#include "adaption_cln/hash.h"
#include "adaption_cln/operations.h"
#include "adaption_cln/typetraits.h"
#endif

#ifdef USE_MPFR_FLOAT
#include <mpfr.h>
#endif

#include "adaption_float/FLOAT_T.h"
#include "adaption_float/typetraits.h"

#include "GaloisField.h"
#include "GFNumber.h"

#include "conversion/conversion.h"
