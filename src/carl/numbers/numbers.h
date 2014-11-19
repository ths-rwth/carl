/** 
 * @file:   numbers.h
 * @author: Sebastian Junges
 *
 * @since August 26, 2013
 */

#pragma once

#include "typetraits.h"

#include "adaption_cln/hash.h"
#include "adaption_cln/operations.h"
#include "adaption_cln/typetraits.h"
#include "adaption_cln/constants.h"

#include "adaption_float/typetraits.h"

#include "adaption_gmpxx/hash.h"
#include "adaption_gmpxx/operations.h"
#include "adaption_gmpxx/typetraits.h"
#include "adaption_gmpxx/constants.h"

#include "adaption_native/operations.h"
#include "adaption_native/typetraits.h"

#ifdef USE_Z3_NUMBERS
#include "adaption_z3/z3_include.h"
#include "adaption_z3/hash.h"
#include "adaption_z3/operations.h"
#include "adaption_z3/typetraits.h"
#include "adaption_z3/constants.h"
#endif

#include "GaloisField.h"
#include "GFNumber.h"

#include "Numeric.h"

#include "constants.h"
#include "operations.h"