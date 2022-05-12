#include "../Common.h"

#include <carl/poly/umvpoly/Monomial.h>
#include <carl/groebner/Ideal.h>
#include <carl/groebner/Reductor.h>
#include <carl-common/meta/platform.h>

#include <gtest/gtest.h>


using namespace carl;

TEST(Ideal, Construction)
{
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    Variable z = freshRealVariable("z");
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
