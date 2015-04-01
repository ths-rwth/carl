#include "gtest/gtest.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/Resultant.h"

#include <random>
#include <cmath>
using namespace carl;

#ifdef USE_CLN_NUMBERS
#include <cln/cln.h>
typedef cln::cl_RA Rational;
typedef cln::cl_I Integer;
#else
#include <gmpxx.h>
typedef mpq_class Rational;
typedef mpz_class Integer;
#endif

TEST(Resultant, det)
{
    Variable m = VariablePool::getInstance().getFreshVariable("m");
    Variable r = VariablePool::getInstance().getFreshVariable("r");

    MultivariatePolynomial<Rational> mr(r);
    MultivariatePolynomial<Rational> one((Rational)1);
    MultivariatePolynomial<Rational> zero((Rational)0);
    Variable x = VariablePool::getInstance().getFreshVariable("x");
	Variable c = VariablePool::getInstance().getFreshVariable("c");
	Variable t = VariablePool::getInstance().getFreshVariable("t");

	MultivariatePolynomial<Rational> mc(c);
	MultivariatePolynomial<Rational> mt(t);

    //UnivariatePolynomial<MultivariatePolynomial<Rational>> p(m, {mr+one, zero, -one});
    //UnivariatePolynomial<MultivariatePolynomial<Rational>> q(m, {1, 4, 2});
    //UnivariatePolynomial<MultivariatePolynomial<Rational>> res(r, {833, -124, 4});
    UnivariatePolynomial<MultivariatePolynomial<Rational>> p(x, {mt, mc, mc});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> q(x, {(Rational)0, (Rational)0, (Rational)0, (Rational)0, (Rational)1});
	UnivariatePolynomial<MultivariatePolynomial<Rational>> res(x, MultivariatePolynomial<Rational>(Term<Rational>(t)*t*t*t));

    Resultant calc;

    auto r1 = calc.resultant_det(p, q);
    auto r2 = p.resultant(q);
    auto r3 = calc.resultant_z3(p,q);

    //EXPECT_EQ(r2, r1);
    //EXPECT_EQ(r3, r1);
    //EXPECT_EQ(r3, r2);
}
