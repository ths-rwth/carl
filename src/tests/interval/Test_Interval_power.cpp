#include "../Common.h"

#include <carl/interval/Interval.h>
#include <carl/interval/power.h>

using I = carl::Interval<double>;

#define EXPECT_RESULT(F,IN,R) \
	EXPECT_TRUE((I(R,R) + I(-0.01,0.01)).contains(F(I(IN,IN))))
#define EXPECT_RESULT_E(F,IN,EXP,R) \
	EXPECT_TRUE((I(R,R) + I(-0.01,0.01)).contains(F(I(IN,IN),EXP)))

TEST(Interval, pow) {
	EXPECT_RESULT_E(carl::pow, 0.0, 2, 0.0);
	EXPECT_RESULT_E(carl::pow, 1.0, 2, 1.0);
	EXPECT_RESULT_E(carl::pow, 2.0, 2, 4.0);
	EXPECT_RESULT_E(carl::pow, 3.0, 2, 9.0);
	EXPECT_RESULT_E(carl::pow, 4.0, 2, 16.0);
	EXPECT_RESULT_E(carl::pow, 5.0, 2, 25.0);
	EXPECT_RESULT_E(carl::pow, 6.0, 2, 36.0);
}

TEST(Interval, sqrt) {
	EXPECT_RESULT(carl::sqrt, 0.0, 0.0);
	EXPECT_RESULT(carl::sqrt, 1.0, 1.0);
	EXPECT_RESULT(carl::sqrt, 4.0, 2.0);
	EXPECT_RESULT(carl::sqrt, 9.0, 3.0);
	EXPECT_RESULT(carl::sqrt, 16.0, 4.0);
	EXPECT_RESULT(carl::sqrt, 25.0, 5.0);
	EXPECT_RESULT(carl::sqrt, 36.0, 6.0);
}
