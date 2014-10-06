#include "gtest/gtest.h"

#include "../../carl/numbers/numbers.h"

#include <cln/cln.h>

using namespace carl;

TEST(NumbersCLN, constructors)
{
	cln::cl_RA b = cln::cl_RA(2)/3;
	EXPECT_EQ(2, cln::numerator(b));
	EXPECT_EQ(3, cln::denominator(b));
}

TEST(NumbersCLN, operations)
{
	EXPECT_EQ((cln::cl_I)1, carl::floor(cln::rationalize(1.5)));
	EXPECT_EQ((cln::cl_I)2, carl::ceil(cln::rationalize(1.5)));
	
	EXPECT_EQ((cln::cl_I)5, carl::gcd((cln::cl_I)15, (cln::cl_I)20));
}

TEST(NumbersCLN, squareroot)
{
	cln::cl_RA b = cln::cl_I(2)/cln::cl_I(3);
	std::pair<cln::cl_RA, cln::cl_RA> resultB;
	resultB = carl::sqrt(b);

}

TEST(NumbersCLN, sqrt_fast)
{
	{
		std::pair<cln::cl_RA,cln::cl_RA> s = carl::sqrt_fast(cln::cl_I("64"));
		EXPECT_EQ(cln::cl_RA(8), s.first);
		EXPECT_EQ(cln::cl_RA(8), s.second);
	}
	{
		std::pair<cln::cl_RA,cln::cl_RA> s = carl::sqrt_fast(cln::cl_I("448"));
		EXPECT_EQ(cln::cl_RA(21), s.first);
		EXPECT_EQ(cln::cl_RA(22), s.second);
	}

}

TEST(NumbersCLN, rationalize)
{
	EXPECT_EQ(cln::cl_RA(0), rationalize<cln::cl_RA>("0"));
	EXPECT_EQ(cln::cl_RA(1)/cln::cl_RA(10), rationalize<cln::cl_RA>("0.1"));
	EXPECT_EQ(cln::cl_RA(1)/cln::cl_RA(10), rationalize<cln::cl_RA>(".1"));
	EXPECT_EQ(cln::cl_RA(3)/cln::cl_RA(2), rationalize<cln::cl_RA>("1.5"));
	EXPECT_EQ(cln::cl_RA(1234567890), rationalize<cln::cl_RA>("1234567890"));
}
