/**
 * @file z3_include.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */
#pragma once

// Prevent Z3 from using openmp
#define _NO_OMP_
#define uint64 unsigned long
typedef unsigned digit_t;

#include <util/mpz.h>
#include <util/mpq.h>
#include <math/polynomial/polynomial.h>
#include <util/vector.h>
