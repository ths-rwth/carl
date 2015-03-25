#include "gtest/gtest.h"
#include "carl/core/UnivariatePolynomial.h"
#include "carl/core/VariablePool.h"
#include "carl/core/MultivariatePolynomial.h"
#include "carl/core/Resultant.h"

#include <cln/cln.h>
#include <gmpxx.h>
#include <random>
#include <cmath>
using namespace carl;

TEST(Resultant, det)
{
    Variable m = VariablePool::getInstance().getFreshVariable("m");
    Variable r = VariablePool::getInstance().getFreshVariable("r");

    MultivariatePolynomial<cln::cl_RA> mr(r);
    MultivariatePolynomial<cln::cl_RA> one((cln::cl_RA)1);
    MultivariatePolynomial<cln::cl_RA> zero((cln::cl_RA)0);
    Variable x = VariablePool::getInstance().getFreshVariable("x");
	Variable c = VariablePool::getInstance().getFreshVariable("c");
	Variable t = VariablePool::getInstance().getFreshVariable("t");

	MultivariatePolynomial<cln::cl_RA> mc(c);
	MultivariatePolynomial<cln::cl_RA> mt(t);

    //UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>> p(m, {mr+one, zero, -one});
    //UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>> q(m, {1, 4, 2});
    //UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>> res(r, {833, -124, 4});
    UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>> p(x, {mt, mc, mc});
	UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>> q(x, {(cln::cl_RA)0, (cln::cl_RA)0, (cln::cl_RA)0, (cln::cl_RA)0, (cln::cl_RA)1});
	UnivariatePolynomial<MultivariatePolynomial<cln::cl_RA>> res(x, MultivariatePolynomial<cln::cl_RA>(Term<cln::cl_RA>(t)*t*t*t));

    Resultant calc;

    auto r1 = calc.resultant_det(p, q);
    auto r2 = p.resultant(q);
    auto r3 = calc.resultant_z3(p,q);

    //EXPECT_EQ(r2, r1);
    //EXPECT_EQ(r3, r1);
    EXPECT_EQ(r3, r2);
}
