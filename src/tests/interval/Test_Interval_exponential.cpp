#include <gtest/gtest.h>

#include <carl-arith/interval/Interval.h>
#include <carl-arith/interval/Exponential.h>

using I = carl::Interval<double>;

#define EXPECT_RESULT(F,IN,R) \
	EXPECT_TRUE((I(R,R) + I(-0.01,0.01)).contains(F(I(IN,IN))))

TEST(Interval, exp) {
	EXPECT_RESULT(carl::exp, -5.0, 0.007);
	EXPECT_RESULT(carl::exp, -4.0, 0.018);
	EXPECT_RESULT(carl::exp, -3.0, 0.050);
	EXPECT_RESULT(carl::exp, -2.0, 0.135);
	EXPECT_RESULT(carl::exp, -1.0, 0.368);
	EXPECT_RESULT(carl::exp, 0.0, 1.0);
	EXPECT_RESULT(carl::exp, 1.0, 2.718);
	EXPECT_RESULT(carl::exp, 2.0, 7.389);
	EXPECT_RESULT(carl::exp, 3.0, 20.086);
	EXPECT_RESULT(carl::exp, 4.0, 54.598);
	EXPECT_RESULT(carl::exp, 5.0, 148.413);
}

TEST(Interval, log) {
	EXPECT_RESULT(carl::log, 1.0, 0.0);
	EXPECT_RESULT(carl::log, 2.0, 0.693);
	EXPECT_RESULT(carl::log, 3.0, 1.099);
	EXPECT_RESULT(carl::log, 4.0, 1.386);
	EXPECT_RESULT(carl::log, 5.0, 1.609);
}
