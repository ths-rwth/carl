#include "gtest/gtest.h"
#include "carl/groebner/Reductor.h"

#include "carl/core/Monomial.h"

#include "carl/groebner/Ideal.h"

#include "../Common.h"


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
