#include "gtest/gtest.h"

#include "../../carl/numbers/numbers.h"
#include "../../carl/numbers/DecimalStringToRational.h"

#include <gmpxx.h>

using namespace carl;

TEST(NumbersGMPxx, constructors)
{
	mpq_class a(mpz_class(2), mpz_class(3));
	EXPECT_EQ(a.get_den(), 3);
	EXPECT_EQ(a.get_num(), 2);
    mpq_class b("3");
}

TEST(NumbersGMPxx, squareroot)
{
	mpq_class a(mpz_class(2), mpz_class(3));
	std::pair<mpq_class, mpq_class> resultA;
	resultA = sqrt(a);
	EXPECT_EQ(resultA.first.get_den(), 2);
	EXPECT_EQ(resultA.first.get_num(), 1);
	EXPECT_EQ(resultA.second.get_den(), 1);
	EXPECT_EQ(resultA.second.get_num(), 2);
}

TEST(NumbersGMPxx, quotient)
{
	EXPECT_EQ(-1, quotient(mpz_class(-6),mpz_class(7)));
}

TEST(NumbersGMPxx, rationalize)
{
	EXPECT_EQ(mpq_class(0), rationalize<mpq_class>("0"));
	EXPECT_EQ(mpq_class(1,10), rationalize<mpq_class>("0.1"));
	EXPECT_EQ(mpq_class(1,10), rationalize<mpq_class>(".1"));
	EXPECT_EQ(mpq_class(3,2), rationalize<mpq_class>("1.5"));
	EXPECT_EQ(mpq_class(1234567890), rationalize<mpq_class>("1234567890"));
}
