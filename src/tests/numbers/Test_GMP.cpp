#include "gtest/gtest.h"
#include "../../carl/numbers/numbers.h"

TEST(GMP, Debug)
{
	carl::sint i = 4;
	mpq_class n(static_cast<signed long>(i));
}

TEST(GMP, Rationalize)
{
	EXPECT_TRUE( carl::rationalize<mpq_class>(carl::toDouble(mpq_class(1)/mpq_class(3))) != mpq_class(1)/mpq_class(3) );
	EXPECT_TRUE( carl::rationalize<mpq_class>(carl::toDouble(mpq_class(1)/mpq_class(20))) != mpq_class(1)/mpq_class(20) );

	EXPECT_EQ(carl::parse<mpq_class>("1"), mpq_class(1));
	EXPECT_EQ(carl::parse<mpq_class>("123"), mpq_class(123));
	EXPECT_EQ(carl::parse<mpq_class>("1e3"), mpq_class(1000));
	EXPECT_EQ(carl::parse<mpq_class>("0.1"), mpq_class(1)/mpq_class(10));
	EXPECT_EQ(carl::parse<mpq_class>("1/2"), mpq_class(1)/mpq_class(2));
	EXPECT_EQ(carl::parse<mpq_class>("0.25/0.125"), mpq_class(2));
}
