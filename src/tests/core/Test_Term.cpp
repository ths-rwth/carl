#include "gtest/gtest.h"
#include "../numbers/config.h"
#include "carl/core/Variable.h"
#include "carl/core/Monomial.h"
#include "carl/core/Term.h"

using namespace carl;

template<typename T>
class TermTest: public testing::Test {};

TYPED_TEST_CASE(TermTest, RationalTypes); // should use NumberTypes

TYPED_TEST(TermTest, Constructor)
{
    Term<TypeParam> t(1);
    Variable v0(1);
    Monomial m0(v0);
    Term<TypeParam> t0(m0);
}

TYPED_TEST(TermTest, Operators)
{
    Term<TypeParam> t(1);
    Variable v0(1);
    Monomial m0(v0);
    Term<TypeParam> t0(m0);
    EXPECT_TRUE(t0.isLinear());
    EXPECT_TRUE(t.isLinear());
    EXPECT_EQ((unsigned)0, t.getNrVariables());
}

TYPED_TEST(TermTest, Multiplication)
{
    Term<TypeParam> t(1);
    Variable v0(1);
    t *= v0;
    Term<TypeParam> t0(v0);
    EXPECT_EQ(t0,t);
    t *= v0;
    Term<TypeParam> t1(Monomial(v0,2));
    EXPECT_EQ(t1,t);
    
}

TYPED_TEST(TermTest, Derivative)
{
    Variable v0(1);
    Variable v1(2);
    Term<TypeParam> t(3);
    t *= v0 * v0 * v0 * v1;
    Term<TypeParam>* tprime = t.derivative(v0);
    EXPECT_EQ(9,tprime->coeff());
}

TYPED_TEST(TermTest, Substitute)
{
    Variable v0(1);
    Variable v1(2);
    Term<TypeParam> t(3);
    Term<TypeParam> c4(4);
    t *= v0 * v0 * v1;
    std::map<Variable, Term<TypeParam>> substitutes;
    substitutes.emplace(v1, c4);
    Term<TypeParam>* res = t.substitute(substitutes);
    EXPECT_EQ(12, res->coeff());
    EXPECT_EQ((unsigned)2, res->tdeg());
}

TYPED_TEST(TermTest, Comparison)
{
    Variable v0(1);
    Variable v1(2);
    Variable v2(3);

    Term<TypeParam> t0(3, v0 * v0 * v1); // 1.
    Term<TypeParam> t1(7, v1 * v1 * v1 * v2); // 3.
    Term<TypeParam> t2(2, v0 * v1 * v2); // 0.
    Term<TypeParam> t3(9, v0 * v0 * v1); // 2.

    // lower than
    EXPECT_FALSE(t0 < t0);
    EXPECT_TRUE(t0 < t1);
    EXPECT_FALSE(t0 < t2);
    EXPECT_TRUE(t0 < t3);

    EXPECT_FALSE(t1 < t0);
    EXPECT_FALSE(t1 < t1);
    EXPECT_FALSE(t1 < t2);
    EXPECT_FALSE(t1 < t3);

    EXPECT_TRUE(t2 < t0);
    EXPECT_TRUE(t2 < t1);
    EXPECT_FALSE(t2 < t2);
    EXPECT_TRUE(t2 < t3);

    EXPECT_FALSE(t3 < t0);
    EXPECT_TRUE(t3 < t1);
    EXPECT_FALSE(t3 < t2);
    EXPECT_FALSE(t3 < t3);

    // greater than
    EXPECT_FALSE(t0 > t0);
    EXPECT_FALSE(t0 > t1);
    EXPECT_TRUE(t0 > t2);
    EXPECT_FALSE(t0 > t3);

    EXPECT_TRUE(t1 > t0);
    EXPECT_FALSE(t1 > t1);
    EXPECT_TRUE(t1 > t2);
    EXPECT_TRUE(t1 > t3);

    EXPECT_FALSE(t2 > t0);
    EXPECT_FALSE(t2 > t1);
    EXPECT_FALSE(t2 > t2);
    EXPECT_FALSE(t2 > t3);

    EXPECT_TRUE(t3 > t0);
    EXPECT_FALSE(t3 > t1);
    EXPECT_TRUE(t3 > t2);
    EXPECT_FALSE(t3 > t3);

    // lower than or equal
    EXPECT_TRUE(t0 <= t0);
    EXPECT_TRUE(t0 <= t1);
    EXPECT_FALSE(t0 <= t2);
    EXPECT_TRUE(t0 <= t3);

    EXPECT_FALSE(t1 <= t0);
    EXPECT_TRUE(t1 <= t1);
    EXPECT_FALSE(t1 <= t2);
    EXPECT_FALSE(t1 <= t3);

    EXPECT_TRUE(t2 <= t0);
    EXPECT_TRUE(t2 <= t1);
    EXPECT_TRUE(t2 <= t2);
    EXPECT_TRUE(t2 <= t3);

    EXPECT_FALSE(t3 <= t0);
    EXPECT_TRUE(t3 <= t1);
    EXPECT_FALSE(t3 <= t2);
    EXPECT_TRUE(t3 <= t3);

    // greather than or equal
    EXPECT_TRUE(t0 >= t0);
    EXPECT_FALSE(t0 >= t1);
    EXPECT_TRUE(t0 >= t2);
    EXPECT_FALSE(t0 >= t3);

    EXPECT_TRUE(t1 >= t0);
    EXPECT_TRUE(t1 >= t1);
    EXPECT_TRUE(t1 >= t2);
    EXPECT_TRUE(t1 >= t3);

    EXPECT_FALSE(t2 >= t0);
    EXPECT_FALSE(t2 >= t1);
    EXPECT_TRUE(t2 >= t2);
    EXPECT_FALSE(t2 >= t3);

    EXPECT_TRUE(t3 >= t0);
    EXPECT_FALSE(t3 >= t1);
    EXPECT_TRUE(t3 >= t2);
    EXPECT_TRUE(t3 >= t3);

    // equal
    EXPECT_TRUE(t0 == t0);
    EXPECT_FALSE(t0 == t1);
    EXPECT_FALSE(t0 == t2);
    EXPECT_FALSE(t0 == t3);

    EXPECT_FALSE(t1 == t0);
    EXPECT_TRUE(t1 == t1);
    EXPECT_FALSE(t1 == t2);
    EXPECT_FALSE(t1 == t3);

    EXPECT_FALSE(t2 == t0);
    EXPECT_FALSE(t2 == t1);
    EXPECT_TRUE(t2 == t2);
    EXPECT_FALSE(t2 == t3);

    EXPECT_FALSE(t3 == t0);
    EXPECT_FALSE(t3 == t1);
    EXPECT_FALSE(t3 == t2);
    EXPECT_TRUE(t3 == t3);
}
