#include "gtest/gtest.h"

#include "../../carl/numbers/numbers.h"
#include "config.h"

#include <gmpxx.h>

using namespace carl;

template<typename T>
class IntegerNumbers: public testing::Test {};

TYPED_TEST_CASE(IntegerNumbers, IntegerTypes);

TYPED_TEST(IntegerNumbers, Division) {
  TypeParam quot;
  TypeParam rema;
  divide(TypeParam(10), TypeParam(3), quot, rema);
  EXPECT_EQ(TypeParam(3), quot);
  EXPECT_EQ(TypeParam(1), rema);
}

TYPED_TEST(IntegerNumbers, Quotient) {
  EXPECT_EQ(0, quotient(TypeParam(-6), TypeParam(7)));
}

TYPED_TEST(IntegerNumbers, Pow) {
  EXPECT_EQ(TypeParam(1), TypeParam(pow(0, 0)));
	EXPECT_EQ(TypeParam(0), TypeParam(pow(0, 1)));
	EXPECT_EQ(TypeParam(0), TypeParam(pow(0, 2)));
	EXPECT_EQ(TypeParam(1), TypeParam(pow(1, 0)));
	EXPECT_EQ(TypeParam(1), TypeParam(pow(1, 1)));
	EXPECT_EQ(TypeParam(1), TypeParam(pow(1, 2)));
	EXPECT_EQ(TypeParam(1), TypeParam(pow(2, 0)));
	EXPECT_EQ(TypeParam(2), TypeParam(pow(2, 1)));
	EXPECT_EQ(TypeParam(4), TypeParam(pow(2, 2)));
	EXPECT_EQ(TypeParam(27), TypeParam(pow(3, 3)));
	EXPECT_EQ(TypeParam(81), TypeParam(pow(3, 4)));
	EXPECT_EQ(TypeParam(243), TypeParam(pow(3, 5)));
}

/*TYPED_TEST(IntegerNumbers, HighestPower) { // not working because of sizeOf for not-native
  EXPECT_EQ(TypeParam(64), highestPower(TypeParam(64)));
  EXPECT_EQ(TypeParam(64), highestPower(TypeParam(65)));
  EXPECT_EQ(TypeParam(64), highestPower(TypeParam(66)));
  EXPECT_EQ(TypeParam(0), highestPower(TypeParam(0)));
}*/

TYPED_TEST(IntegerNumbers, Constant_Neg_Pos) {
  EXPECT_EQ(true, isZero(TypeParam(0)));
  EXPECT_EQ(false, isZero(TypeParam(1)));
  EXPECT_EQ(false, isZero(TypeParam(-1)));

  EXPECT_EQ(true, isOne(TypeParam(1)));
  EXPECT_EQ(false, isOne(TypeParam(2)));
  EXPECT_EQ(false, isOne(TypeParam(0)));
  EXPECT_EQ(false, isOne(TypeParam(-1)));

  EXPECT_EQ(true, isPositive(TypeParam(1)));
  EXPECT_EQ(false, isPositive(TypeParam(0)));
  EXPECT_EQ(false, isPositive(TypeParam(-1)));

  EXPECT_EQ(true, isNegative(TypeParam(-1)));
  EXPECT_EQ(false, isNegative(TypeParam(0)));
  EXPECT_EQ(false, isNegative(TypeParam(1)));
}

/*
 *                                Integer
// ---------- --------- -------- ------- ------ ----- ---- --- -- -
// ---------- --------- -------- ------- ------ ----- ---- --- -- -
 *                                Rational
 */

template<typename T>
class RationalNumbers: public testing::Test {};

TYPED_TEST_CASE(RationalNumbers, RationalTypes);

TYPED_TEST(RationalNumbers, Constructors) {
	TypeParam a = TypeParam(2)/TypeParam(3);
  EXPECT_EQ(2, getNum(a));
  EXPECT_EQ(3, getDenom(a));
}

TYPED_TEST(RationalNumbers, Squareroot) {
	TypeParam a = TypeParam(2)/TypeParam(3);
  std::pair<TypeParam, TypeParam> resultA = sqrt_safe(a);
  EXPECT_EQ(1, getNum(resultA.first));
  EXPECT_EQ(2, getDenom(resultA.first));
  EXPECT_EQ(2, getNum(resultA.second));
  EXPECT_EQ(1, getDenom(resultA.second));
}

TYPED_TEST(RationalNumbers, Rationalize) {
  EXPECT_EQ(TypeParam(0), rationalize<TypeParam>("0"));
  EXPECT_EQ(TypeParam(1)/TypeParam(10), rationalize<TypeParam>("0.1"));
  EXPECT_EQ(TypeParam(1)/TypeParam(10), rationalize<TypeParam>(".1"));
  EXPECT_EQ(TypeParam(3)/TypeParam(2), rationalize<TypeParam>("1.5"));
  EXPECT_EQ(TypeParam(1234567890), rationalize<TypeParam>("1234567890"));
}

TYPED_TEST(RationalNumbers, Operations) {
  EXPECT_EQ(TypeParam(1), floor(rationalize<TypeParam>("1.5")));
  EXPECT_EQ(TypeParam(2), ceil(rationalize<TypeParam>("1.5")));

  EXPECT_EQ(TypeParam(5), gcd(TypeParam(15), TypeParam(20)));
}

TYPED_TEST(RationalNumbers, Sqrt_fast) {
  {
    std::pair<TypeParam, TypeParam> s = sqrt_fast(TypeParam(64));
    EXPECT_EQ(TypeParam(8), s.first);
    EXPECT_EQ(TypeParam(8), s.second);
  }
  {
    std::pair<TypeParam, TypeParam> s = sqrt_fast(TypeParam(448));
    EXPECT_EQ(TypeParam(21), s.first);
    EXPECT_EQ(TypeParam(22), s.second);
  }
}

TYPED_TEST(RationalNumbers, Constant_Neg_Pos) {
  EXPECT_EQ(true, isZero(TypeParam(0)));
  EXPECT_EQ(false, isZero(TypeParam(1)/TypeParam(10)));
  EXPECT_EQ(false, isZero(TypeParam(-1)/TypeParam(10)));

  EXPECT_EQ(true, isOne(TypeParam(1)));
  EXPECT_EQ(true, isOne(TypeParam(2)/TypeParam(2)));
  EXPECT_EQ(false, isOne(TypeParam(11)/TypeParam(10)));
  EXPECT_EQ(false, isOne(TypeParam(0)));
  EXPECT_EQ(false, isOne(TypeParam(-1)/TypeParam(10)));

  EXPECT_EQ(true, isPositive(TypeParam(1)/TypeParam(10)));
  EXPECT_EQ(false, isPositive(TypeParam(0)));
  EXPECT_EQ(false, isPositive(TypeParam(-1)/TypeParam(10)));

  EXPECT_EQ(true, isNegative(TypeParam(-1)/TypeParam(10)));
  EXPECT_EQ(false, isNegative(TypeParam(0)));
  EXPECT_EQ(false, isNegative(TypeParam(1)/TypeParam(10)));
}

TYPED_TEST(RationalNumbers, ToDouble) {
  double res = toDouble(TypeParam(1)/TypeParam(10));
  EXPECT_EQ(true, res>0.05);
  EXPECT_EQ(true, res<0.15);
}

TYPED_TEST(RationalNumbers, ToInt) {
  typedef typename IntegralType<TypeParam>::type IntType;
  EXPECT_EQ(42, toInt<IntType>(TypeParam(42)/TypeParam(1)));
}
