#include "gtest/gtest.h"
#include "carl/groebner/Reductor.h"

#include "carl/core/Monomial.h"
#include "carl/core/MultivariatePolynomial.h"

#include "carl/groebner/Ideal.h"

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

using namespace carl;

TEST(Ideal, Construction)
{  
    VariablePool& vpool = VariablePool::getInstance();
    Variable x = vpool.getFreshVariable();
    Variable y = vpool.getFreshVariable();
    Variable z = vpool.getFreshVariable();
    vpool.setName(z, "z");
    Ideal<MultivariatePolynomial<Rational>> ideal;  
    MultivariatePolynomial<Rational> p1;
    p1 += x*x;
    p1 += z;
    MultivariatePolynomial<Rational> p2;
    p2 += y*y;
    ideal.addGenerator(p1);
    ideal.addGenerator(p2);
    ideal.print();
}



