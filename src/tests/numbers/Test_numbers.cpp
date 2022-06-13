#include <carl-arith/numbers/numbers.h>
#include <gtest/gtest.h>

#include "../number_types.h"

template<typename T>
class IntegerNumbers: public testing::Test {};

TYPED_TEST_CASE(IntegerNumbers, IntegerTypes);

TYPED_TEST(IntegerNumbers, Division) {
  TypeParam quot;
  TypeParam rema;
  carl::divide(TypeParam(10), TypeParam(3), quot, rema);
  EXPECT_EQ(TypeParam(3), quot);
  EXPECT_EQ(TypeParam(1), rema);
}

TYPED_TEST(IntegerNumbers, Quotient) {
  EXPECT_EQ(0, carl::quotient(TypeParam(-6), TypeParam(7)));
}

TYPED_TEST(IntegerNumbers, Pow) {
  EXPECT_EQ(TypeParam(1), TypeParam(carl::pow(0, 0)));
	EXPECT_EQ(TypeParam(0), TypeParam(carl::pow(0, 1)));
	EXPECT_EQ(TypeParam(0), TypeParam(carl::pow(0, 2)));
	EXPECT_EQ(TypeParam(1), TypeParam(carl::pow(1, 0)));
	EXPECT_EQ(TypeParam(1), TypeParam(carl::pow(1, 1)));
	EXPECT_EQ(TypeParam(1), TypeParam(carl::pow(1, 2)));
	EXPECT_EQ(TypeParam(1), TypeParam(carl::pow(2, 0)));
	EXPECT_EQ(TypeParam(2), TypeParam(carl::pow(2, 1)));
	EXPECT_EQ(TypeParam(4), TypeParam(carl::pow(2, 2)));
	EXPECT_EQ(TypeParam(27), TypeParam(carl::pow(3, 3)));
	EXPECT_EQ(TypeParam(81), TypeParam(carl::pow(3, 4)));
	EXPECT_EQ(TypeParam(243), TypeParam(carl::pow(3, 5)));
}

/*TYPED_TEST(IntegerNumbers, HighestPower) { // not working because of sizeOf for not-native
  EXPECT_EQ(TypeParam(64), highestPower(TypeParam(64)));
  EXPECT_EQ(TypeParam(64), highestPower(TypeParam(65)));
  EXPECT_EQ(TypeParam(64), highestPower(TypeParam(66)));
  EXPECT_EQ(TypeParam(0), highestPower(TypeParam(0)));
}*/

TYPED_TEST(IntegerNumbers, Constant_Neg_Pos) {
  EXPECT_EQ(true, carl::is_zero(TypeParam(0)));
  EXPECT_EQ(false, carl::is_zero(TypeParam(1)));
  EXPECT_EQ(false, carl::is_zero(TypeParam(-1)));

  EXPECT_EQ(true, carl::is_one(TypeParam(1)));
  EXPECT_EQ(false, carl::is_one(TypeParam(2)));
  EXPECT_EQ(false, carl::is_one(TypeParam(0)));
  EXPECT_EQ(false, carl::is_one(TypeParam(-1)));

  EXPECT_EQ(true, carl::is_positive(TypeParam(1)));
  EXPECT_EQ(false, carl::is_positive(TypeParam(0)));
  EXPECT_EQ(false, carl::is_positive(TypeParam(-1)));

  EXPECT_EQ(true, carl::is_negative(TypeParam(-1)));
  EXPECT_EQ(false, carl::is_negative(TypeParam(0)));
  EXPECT_EQ(false, carl::is_negative(TypeParam(1)));
}

//TYPED_TEST(IntegerNumbers, gdc_lcm) { // not working because of native int
//  TypeParam a = TypeParam(15), b = TypeParam(20);
//  div_assign(a, b);
//  gcd_assign(a,b);
//  EXPECT_EQ(TypeParam(5), a);

//  EXPECT_EQ(TypeParam(72), lcm(TypeParam(24),TypeParam(36)));
//}

/*
 *                                Integer
 * ---------- --------- -------- ------- ------ ----- ---- --- -- -
 * ---------- --------- -------- ------- ------ ----- ---- --- -- -
 *                                Rational
 */

template<typename T>
class RationalNumbers: public testing::Test {};

TYPED_TEST_CASE(RationalNumbers, RationalTypes);

TYPED_TEST(RationalNumbers, Constructors) {
	TypeParam a = TypeParam(2)/TypeParam(3);
	EXPECT_EQ(2, carl::get_num(a));
	EXPECT_EQ(3, carl::get_denom(a));
}

TYPED_TEST(RationalNumbers, Squareroot) {
	{
		TypeParam a = TypeParam(2);
		std::pair<TypeParam, TypeParam> res = carl::sqrt_safe(a);
		EXPECT_LE(res.first*res.first, a);
		EXPECT_LE(a, res.second*res.second);
	}
	{
		TypeParam a = TypeParam(2)/TypeParam(3);
		std::pair<TypeParam, TypeParam> res = carl::sqrt_safe(a);
		EXPECT_LE(res.first*res.first, a);
		EXPECT_LE(a, res.second*res.second);
	}
	{
		TypeParam a = TypeParam("93536104789177766012087302264675950042191285291185")/TypeParam("93536104789177786765035829293842113257979682750464");
		std::pair<TypeParam, TypeParam> resultA = carl::sqrt_safe(a);
		EXPECT_LE(resultA.first*resultA.first, a);
		EXPECT_LE(a, resultA.second*resultA.second);
	}
}

TYPED_TEST(RationalNumbers, Sqrt_fast) {
  {
    std::pair<TypeParam, TypeParam> s = carl::sqrt_fast(TypeParam(64));
    EXPECT_EQ(TypeParam(8), s.first);
    EXPECT_EQ(TypeParam(8), s.second);
  }
  {
    std::pair<TypeParam, TypeParam> s = carl::sqrt_fast(TypeParam(448));
    EXPECT_EQ(TypeParam(21), s.first);
    EXPECT_EQ(TypeParam(22), s.second);
  }
  {
		auto s = carl::sqrt_fast(TypeParam(9)/4);
    EXPECT_EQ(s.first, TypeParam(3)/2);
    EXPECT_EQ(s.second, TypeParam(3)/2);
	}
}

TYPED_TEST(RationalNumbers, Squareroot_exact) {
	TypeParam res;
  EXPECT_EQ(true, carl::sqrt_exact(TypeParam(9)/TypeParam(16), res));
  EXPECT_EQ(TypeParam(3)/TypeParam(4), res);
  EXPECT_EQ(false, carl::sqrt_exact(TypeParam(2), res));
}

TYPED_TEST(RationalNumbers, Operations) {
  EXPECT_EQ(TypeParam(1), carl::floor(carl::parse<TypeParam>("1.5")));
  EXPECT_EQ(TypeParam(2), carl::ceil(carl::parse<TypeParam>("1.5")));

  EXPECT_EQ(TypeParam(5), carl::gcd(TypeParam(15), TypeParam(20)));
  EXPECT_EQ(TypeParam(72), carl::lcm(TypeParam(24),TypeParam(36)));
  EXPECT_EQ(TypeParam(72), carl::lcm(TypeParam(36),TypeParam(24)));

  TypeParam a = TypeParam(15), b = TypeParam(20);
  carl::gcd_assign(a,b);
  EXPECT_EQ(TypeParam(5), a);

  a = TypeParam(20), b = TypeParam(5);
  carl::div_assign(a,b);
  EXPECT_EQ(TypeParam(4), a);
}

TYPED_TEST(RationalNumbers, log) {
  TypeParam res = carl::log(TypeParam(42));
  EXPECT_GE(res, TypeParam(368)/TypeParam(100));
  EXPECT_LE(res, TypeParam(378)/TypeParam(100));
  res = carl::log(TypeParam(1)/(42));
  EXPECT_GE(res, TypeParam(-378)/TypeParam(100));
  EXPECT_LE(res, TypeParam(-368)/TypeParam(100));

  EXPECT_EQ(TypeParam(0), carl::log(TypeParam(1)));
}

TYPED_TEST(RationalNumbers, log10) {
  TypeParam res = carl::log10(TypeParam(42));
  EXPECT_GE(res, TypeParam(160)/TypeParam(100));
  EXPECT_LE(res, TypeParam(170)/TypeParam(100));
  res = carl::log10(TypeParam(1)/(42));
  EXPECT_GE(res, TypeParam(-170)/TypeParam(100));
  EXPECT_LE(res, TypeParam(-160)/TypeParam(100));

  EXPECT_EQ(TypeParam(0), carl::log10(TypeParam(1)));
}

TYPED_TEST(RationalNumbers, sin_cos) {
  // ---- ---- ---- SIN ---- ---- ----
  TypeParam res = carl::sin(TypeParam(42)); //some number
  EXPECT_GE(res, TypeParam(-96)/TypeParam(100));
  EXPECT_LE(res, TypeParam(-86)/TypeParam(100));
  res = carl::sin(TypeParam(157)/TypeParam(100)); //stay under 1
  EXPECT_GE(res, TypeParam(95)/TypeParam(100));
  EXPECT_LE(res, TypeParam(1));
  res = carl::sin(TypeParam(471)/TypeParam(100)); //stay over -1
  EXPECT_GE(res, TypeParam(-1));
  EXPECT_LE(res, TypeParam(-95)/TypeParam(100));
  EXPECT_EQ(TypeParam(0), carl::sin(TypeParam(0))); // stay exactly on 0

  // ---- ---- ---- COS ---- ---- ----
  res = carl::cos(TypeParam(42)); // some number
  EXPECT_GE(res, TypeParam(-44)/TypeParam(100));
  EXPECT_LE(res, TypeParam(-34)/TypeParam(100));
  EXPECT_EQ(TypeParam(1), carl::cos(TypeParam(0))); // stay exactly on 1
  res = carl::cos(TypeParam(314)/TypeParam(100)); //stay over -1
  EXPECT_GE(res, TypeParam(-1));
  EXPECT_LE(res, TypeParam(-95)/TypeParam(100));
  res = carl::cos(TypeParam(157)/TypeParam(100)); //stay around 0
  EXPECT_GE(res, TypeParam(-5)/TypeParam(100));
  EXPECT_LE(res, TypeParam(5)/TypeParam(100));
}

TYPED_TEST(RationalNumbers, Constant_Neg_Pos) {
  EXPECT_EQ(true, carl::is_zero(TypeParam(0)));
  EXPECT_EQ(false, carl::is_zero(TypeParam(1)/TypeParam(10)));
  EXPECT_EQ(false, carl::is_zero(TypeParam(-1)/TypeParam(10)));

  EXPECT_EQ(true, carl::is_one(TypeParam(1)));
  EXPECT_EQ(true, carl::is_one(TypeParam(2)/TypeParam(2)));
  EXPECT_EQ(false, carl::is_one(TypeParam(11)/TypeParam(10)));
  EXPECT_EQ(false, carl::is_one(TypeParam(0)));
  EXPECT_EQ(false, carl::is_one(TypeParam(-1)/TypeParam(10)));

  EXPECT_EQ(true, carl::is_positive(TypeParam(1)/TypeParam(10)));
  EXPECT_EQ(false, carl::is_positive(TypeParam(0)));
  EXPECT_EQ(false, carl::is_positive(TypeParam(-1)/TypeParam(10)));

  EXPECT_EQ(true, carl::is_negative(TypeParam(-1)/TypeParam(10)));
  EXPECT_EQ(false, carl::is_negative(TypeParam(0)));
  EXPECT_EQ(false, carl::is_negative(TypeParam(1)/TypeParam(10)));
}

TYPED_TEST(RationalNumbers, ToDouble_Int) {
  double res = carl::to_double(TypeParam(1)/TypeParam(10));
  EXPECT_GE(res, 0.05);
  EXPECT_LE(res, 0.15);

  typedef typename carl::IntegralType<TypeParam>::type IntType;
  EXPECT_EQ(42, carl::to_int<IntType>(TypeParam(42)/TypeParam(1)));
  EXPECT_EQ(-42, carl::to_int<IntType>(TypeParam(42)/TypeParam(-1)));
}
