/**
 * @file z3_include.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */
#pragma once

#include "../../io/streamingOperators.h"

using carl::operator<<;

// disable some code in Z3
#define INCLUDED_FROM_CARL

// Prevent Z3 from using openmp
#define _NO_OMP_
#define uint64 unsigned long
typedef unsigned digit_t;
#define _MP_GMP

#include <util/mpz.h>
#include <util/mpq.h>
#include <math/polynomial/polynomial.h>
#include <util/vector.h>
#include <math/polynomial/algebraic_numbers.h>
#include <nlsat/nlsat_assignment.h>
#include <util/gparams.h>

#undef alloc
