#include "gtest/gtest.h"

#include <memory>
#include <list>

#include "carl/cad/ConflictGraph.h"
#include "carl/util/platform.h"

using namespace carl;

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#elif defined(__VS)
#include <mpirxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

TEST(ConflictGraph, BasicOperations)
{  
    cad::ConflictGraph<Rational> cg;
}