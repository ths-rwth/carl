#include "gtest/gtest.h"

#include "../../carl/numbers/numbers.h"
#include "config.h"

using namespace carl;

template<typename T>
class IntegerDivision: public testing::Test {};

TYPED_TEST_CASE(IntegerDivision, IntegerTypes);

TYPED_TEST(IntegerDivision, IntegerDivision) {
	for (int a = -10; a < 11; a++) {
		for (int b = -10; b < 11; b++) {
			if (b == 0) continue;
			if (a % b != 0) continue;
			EXPECT_EQ(TypeParam(a/b), carl::div(TypeParam(a), TypeParam(b)));
		}
	}
}

TYPED_TEST(IntegerDivision, DivisionOperator) {
	for (int a = -10; a < 11; a++) {
		for (int b = -10; b < 11; b++) {
			if (b == 0) continue;
			EXPECT_EQ(TypeParam(a/b), TypeParam(a) / TypeParam(b));
		}
	}
}

TYPED_TEST(IntegerDivision, Quotient) {
	for (int a = -10; a < 11; a++) {
		for (int b = -10; b < 11; b++) {
			if (b == 0) continue;
			EXPECT_EQ(TypeParam(a/b), carl::quotient(TypeParam(a), TypeParam(b)));
		}
	}
}

TYPED_TEST(IntegerDivision, Remainder) {
	for (int a = -10; a < 11; a++) {
		for (int b = -10; b < 11; b++) {
			if (b == 0) continue;
			EXPECT_EQ(TypeParam(a % b), carl::remainder(TypeParam(a), TypeParam(b)));
		}
	}
}

template<typename T>
class RationalDivision: public testing::Test {};

TYPED_TEST_CASE(RationalDivision, RationalTypes);

TYPED_TEST(RationalDivision, Division) {
	for (int a = -10; a < 11; a++) {
		for (int b = -10; b < 11; b++) {
			for (int c = -10; c < 11; c++) {
				for (int d = -10; d < 11; d++) {
					if (b == 0 || c == 0 || d == 0) continue;
					TypeParam n = carl::div(TypeParam(a), TypeParam(b));
					TypeParam m = carl::div(TypeParam(c), TypeParam(d));
					TypeParam r = carl::div(TypeParam(a*d), TypeParam(b*c));
					EXPECT_EQ(r, carl::div(n, m));
					EXPECT_EQ(TypeParam(a*d), carl::div(n, m) * TypeParam(b*c));
				}
			}
		}
	}
}

TYPED_TEST(RationalDivision, DivisionOperator) {
	for (int a = -10; a < 11; a++) {
		for (int b = -10; b < 11; b++) {
			for (int c = -10; c < 11; c++) {
				for (int d = -10; d < 11; d++) {
					if (b == 0 || c == 0 || d == 0) continue;
					TypeParam n = TypeParam(a) / TypeParam(b);
					TypeParam m = TypeParam(c) / TypeParam(d);
					TypeParam r = TypeParam(a*d) / TypeParam(b*c);
					EXPECT_EQ(r, n / m);
					EXPECT_EQ(TypeParam(a*d), n / m * TypeParam(b*c));
				}
			}
		}
	}
}

TYPED_TEST(RationalDivision, Quotient) {
	for (int a = -10; a < 11; a++) {
		for (int b = -10; b < 11; b++) {
			for (int c = -10; c < 11; c++) {
				for (int d = -10; d < 11; d++) {
					if (b == 0 || c == 0 || d == 0) continue;
					TypeParam n = carl::quotient(TypeParam(a), TypeParam(b));
					TypeParam m = carl::quotient(TypeParam(c), TypeParam(d));
					TypeParam r = carl::quotient(TypeParam(a*d), TypeParam(b*c));
					EXPECT_EQ(r, carl::quotient(n, m));
					EXPECT_EQ(TypeParam(a*d), carl::quotient(n, m) * TypeParam(b*c));
				}
			}
		}
	}
}


