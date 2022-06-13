#include <gtest/gtest.h>
#include <carl-arith/poly/umvpoly/functions/Derivative.h>
#include <carl-arith/poly/umvpoly/functions/Substitution.h>
#include <carl-arith/poly/umvpoly/Monomial.h>
#include <carl-arith/poly/umvpoly/Term.h>
#include <carl-arith/core/Variable.h>
#include <carl-arith/core/VariablePool.h>

#include "../number_types.h"
#include "../util.h"

using namespace carl;

template<typename T>
class TermTest: public testing::Test {};

TYPED_TEST_CASE(TermTest, RationalTypes); // TODO should use NumberTypes

TYPED_TEST(TermTest, Constructor)
{
    Term<TypeParam> t(1);
    Variable v0 = carl::fresh_real_variable("v0");
    Term<TypeParam> t0(v0);
}

TYPED_TEST(TermTest, Operators)
{
    Term<TypeParam> t(1);
    Variable v0 = carl::fresh_real_variable("v0");
    Term<TypeParam> t0(v0);
    EXPECT_TRUE(t0.is_linear());
    EXPECT_TRUE(t.is_linear());
    EXPECT_EQ(0u, t.num_variables());
}

TYPED_TEST(TermTest, Multiplication)
{
    Term<TypeParam> t(1);
    Variable v0 = carl::fresh_real_variable("v0");
    t *= v0;
    Term<TypeParam> t0(v0);
    EXPECT_EQ(t0,t);
    t *= v0;
    Term<TypeParam> t1(1, v0,2);
    EXPECT_EQ(t1,t);

}

TYPED_TEST(TermTest, VariableMultiplication)
{
    Variable x = fresh_real_variable("x");
    Variable y = fresh_real_variable("y");

    EXPECT_EQ(Term<TypeParam>(3, x * y),  static_cast<TypeParam>(3) * x * y);
    EXPECT_EQ(Term<TypeParam>(1, x * y), x * y);
    EXPECT_EQ(Term<TypeParam>(-4, x * x * y), x *  static_cast<TypeParam>(-4) * x * y);
}

TYPED_TEST(TermTest, TermMultiplication)
{
    Variable x = fresh_real_variable("x");
    Variable y = fresh_real_variable("y");

    EXPECT_EQ(Term<TypeParam>(12, x * x * y), Term<TypeParam>(3, x * y) * Term<TypeParam>(4, x, 1));
    EXPECT_EQ(Term<TypeParam>(TypeParam(0)), Term<TypeParam>(3, x * x) * Term<TypeParam>(0, y * y * y));
    EXPECT_EQ(Term<TypeParam>(18, x * x * y * y * y), Term<TypeParam>(9, x * y) * Term<TypeParam>(2, x * y * y));
}

TYPED_TEST(TermTest, Derivative)
{
    Variable v0 = carl::fresh_real_variable("v0");
    Variable v1 = carl::fresh_real_variable("v1");
    Term<TypeParam> t(3);
    t *= v0 * v0 * v0 * v1;
    Term<TypeParam> tprime = carl::derivative(t, v0);
    EXPECT_EQ(9,tprime.coeff());
}

TYPED_TEST(TermTest, Substitute)
{
    Variable v0 = carl::fresh_real_variable("v0");
    Variable v1 = carl::fresh_real_variable("v1");
    Term<TypeParam> t(3);
    Term<TypeParam> c4(4);
    t *= v0 * v0 * v1;
    std::map<Variable, Term<TypeParam>> substitutes;
    substitutes.emplace(v1, c4);
    Term<TypeParam> res = carl::substitute(t, substitutes);
    EXPECT_EQ(12, res.coeff());
    EXPECT_EQ((unsigned)2, res.tdeg());
}

TYPED_TEST(TermTest, Comparison)
{
    Variable x = fresh_real_variable("x");
    Variable y = fresh_real_variable("y");
    Variable z = fresh_real_variable("z");

    ComparisonList<Term<TypeParam> > terms;
    terms.push_back( static_cast<TypeParam>(2) * x * y * z);
    terms.push_back( static_cast<TypeParam>(3) * y * z * z);
    terms.push_back( static_cast<TypeParam>(9) * y * z * z);
    terms.push_back( static_cast<TypeParam>(7) * x * y * y * z);
    terms.push_back( static_cast<TypeParam>(4) * x * y * z * z);

    expectRightOrder(terms);
}

TYPED_TEST(TermTest, OtherComparison)
{
    ComparisonList<Variable, Monomial::Arg, Term<TypeParam> > list;

    Variable x = fresh_real_variable("x");
    Variable y = fresh_real_variable("y");

    list.push_back(x);
    list.push_back( static_cast<TypeParam>(7) * x);
    list.push_back(y);
    list.push_back( static_cast<TypeParam>(3) * y);
    list.push_back(x * x);
    list.push_back( static_cast<TypeParam>(3) * x * x);
    list.push_back( static_cast<TypeParam>(9) * x * x * y);
    list.push_back(x * y * y);
    list.push_back( static_cast<TypeParam>(5) * x * y * y);

    expectRightOrder(list);
}
