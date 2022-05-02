#include <gtest/gtest.h>


#include <carl/core/polynomialfunctions/Resultant.h>
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/VariablePool.h"
#include <carl-common/meta/platform.h>

#include <random>
#include <cmath>

#include "../Common.h"
using namespace carl;

TEST(Resultant, det)
{
    //Variable m = freshRealVariable("m");
    Variable r = freshRealVariable("r");

    MultivariatePolynomial<Rational> mr(r);
    MultivariatePolynomial<Rational> one((Rational)1);
    MultivariatePolynomial<Rational> zero((Rational)0);
    Variable x = freshRealVariable("x");
	Variable c = freshRealVariable("c");
	Variable t = freshRealVariable("t");

	MultivariatePolynomial<Rational> mc(c);
	MultivariatePolynomial<Rational> mt(t);

    //UnivariatePolynomial<MultivariatePolynomial<Rational>> p(m, {mr+one, zero, -one});
    //UnivariatePolynomial<MultivariatePolynomial<Rational>> q(m, {1, 4, 2});
    //UnivariatePolynomial<MultivariatePolynomial<Rational>> res(r, {833, -124, 4});
    UnivariatePolynomial<MultivariatePolynomial<Rational>> p(x, {mt, mc, mc});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> q(x, {(Rational)0, (Rational)0, (Rational)0, (Rational)0, (Rational)1});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> res(x, MultivariatePolynomial<Rational>(Term<Rational>(t)*t*t*t));

    auto r1 = carl::resultant_debug::resultant_det(p, q);
    auto r2 = carl::resultant(p, q);
    auto r3 = carl::resultant_debug::resultant_z3(p,q);

    //EXPECT_EQ(r2, r1);
    //EXPECT_EQ(r3, r1);
    //EXPECT_EQ(r3, r2);
}
