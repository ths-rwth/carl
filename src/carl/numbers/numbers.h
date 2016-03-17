/**
 * @file:   numbers.h
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once

#define INCLUDED_FROM_NUMBERS_H true

#include "config.h"
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
#include "adaption_cln/hash.h"
#include "adaption_cln/operations.h"
#include "adaption_cln/typetraits.h"
#endif

#ifdef USE_MPFR_FLOAT
#include <mpfr.h>
#endif

#include "adaption_float/FLOAT_T.h"
#include "adaption_float/typetraits.h"

#ifdef USE_Z3_NUMBERS
#include "adaption_z3/z3_include.h"
#include "adaption_z3/hash.h"
#include "adaption_z3/operations.h"
#include "adaption_z3/typetraits.h"
#endif

#include "GaloisField.h"
#include "GFNumber.h"
#include "Numeric.h"

#include "conversion/conversion.h"

#include "Number.h"
