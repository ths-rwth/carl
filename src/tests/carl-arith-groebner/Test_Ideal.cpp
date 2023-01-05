#include "../Common.h"

#include <carl-arith/poly/umvpoly/Monomial.h>
#include <carl-arith/groebner/Ideal.h>
#include <carl-arith/groebner/Reductor.h>
#include <carl-common/meta/platform.h>

#include <gtest/gtest.h>


using namespace carl;

TEST(Ideal, Construction)
{
    Variable x = fresh_real_variable("x");
    Variable y = fresh_real_variable("y");
    Variable z = fresh_real_variable("z");
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
