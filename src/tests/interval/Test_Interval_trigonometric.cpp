#include <gtest/gtest.h>

#include <carl/interval/Interval.h>
#include <carl/interval/trigonometric.h>

using I = carl::Interval<double>;
constexpr double PI = 3.141592653589793;

#define EXPECT_RESULT(F,IN,R) \
	EXPECT_TRUE((I(R,R) + I(-0.01,0.01)).contains(F(I(IN,IN))))

TEST(Interval, base) {
	EXPECT_RESULT(carl::sin, 0.0, 0.0);
	EXPECT_RESULT(carl::cos, 0.0, 1.0);
	EXPECT_RESULT(carl::tan, 0.0, 0.0);

	EXPECT_RESULT(carl::sin, PI/4, 0.707);
	EXPECT_RESULT(carl::cos, PI/4, 0.707);
	EXPECT_RESULT(carl::tan, PI/4, 1.0);

	EXPECT_RESULT(carl::sin, PI/3, 0.866);
	EXPECT_RESULT(carl::cos, PI/3, 0.5);
	EXPECT_RESULT(carl::tan, PI/3, 1.732);

	EXPECT_RESULT(carl::sin, PI/2, 1.0);
	EXPECT_RESULT(carl::cos, PI/2, 0.0);

	EXPECT_RESULT(carl::sin, PI, 0.0);
	EXPECT_RESULT(carl::cos, PI, -1.0);
}

TEST(Interval, inverse) {
	EXPECT_RESULT(carl::asin, -1.0, -PI/2);
	EXPECT_RESULT(carl::acos, -1.0, PI);
	EXPECT_RESULT(carl::atan, -1.0, -PI/4);

	EXPECT_RESULT(carl::asin, -0.5, -PI/6);
	EXPECT_RESULT(carl::acos, -0.5, 2*PI/3);
	EXPECT_RESULT(carl::atan, -0.5, -0.464);

	EXPECT_RESULT(carl::asin, 0.0, 0.0);
	EXPECT_RESULT(carl::acos, 0.0, PI/2);
	EXPECT_RESULT(carl::atan, 0.0, 0.0);

	EXPECT_RESULT(carl::asin, 0.5, PI/6);
	EXPECT_RESULT(carl::acos, 0.5, PI/3);
	EXPECT_RESULT(carl::atan, 0.5, 0.464);

	EXPECT_RESULT(carl::asin, 1.0, PI/2);
	EXPECT_RESULT(carl::acos, 1.0, 0);
	EXPECT_RESULT(carl::atan, 1.0, PI/4);
}

TEST(Interval, hyperbolic) {
	EXPECT_RESULT(carl::sinh, 0.0, 0.0);
	EXPECT_RESULT(carl::cosh, 0.0, 1.0);
	EXPECT_RESULT(carl::tanh, 0.0, 0.0);

	EXPECT_RESULT(carl::sinh, PI/4, 0.869);
	EXPECT_RESULT(carl::cosh, PI/4, 1.325);
	EXPECT_RESULT(carl::tanh, PI/4, 0.656);

	EXPECT_RESULT(carl::sinh, PI/3, 1.249);
	EXPECT_RESULT(carl::cosh, PI/3, 1.600);
	EXPECT_RESULT(carl::tanh, PI/3, 0.781);

	EXPECT_RESULT(carl::sinh, PI/2, 2.301);
	EXPECT_RESULT(carl::cosh, PI/2, 2.509);
	EXPECT_RESULT(carl::tanh, PI/2, 0.917);

	EXPECT_RESULT(carl::sinh, PI, 11.549);
	EXPECT_RESULT(carl::cosh, PI, 11.592);
	EXPECT_RESULT(carl::tanh, PI, 0.996);
}

TEST(Interval, inverse_hyperbolic) {
	EXPECT_RESULT(carl::asinh, 0.0, 0.0);
	EXPECT_RESULT(carl::atanh, 0.0, 0.0);

	EXPECT_RESULT(carl::atanh, PI/5, 0.739);

	EXPECT_RESULT(carl::asinh, PI/4, 0.721);
	EXPECT_RESULT(carl::atanh, PI/4, 1.059);

	EXPECT_RESULT(carl::asinh, PI/3, 0.914);
	EXPECT_RESULT(carl::acosh, PI/3, 0.306);

	EXPECT_RESULT(carl::asinh, PI/2, 1.233);
	EXPECT_RESULT(carl::acosh, PI/2, 1.023);

	EXPECT_RESULT(carl::asinh, PI, 1.862);
	EXPECT_RESULT(carl::acosh, PI, 1.812);
}