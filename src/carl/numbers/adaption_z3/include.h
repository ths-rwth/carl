/**
 * @file z3_include.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */
#pragma once

// Prevent Z3 from using openmp
#define _NO_OMP_
#define uint64 unsigned long
typedef unsigned digit_t;

#include <z3/src/util/mpz.h>
#include <z3/src/util/mpq.h>
#include <z3/src/math/polynomial/polynomial.h>
#include <z3/src/util/vector.h>
