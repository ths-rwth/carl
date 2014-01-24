#include "gtest/gtest.h"

#include "carl/numbers/typetraits.h"
#include "carl/numbers/operations.h"

#include <cln/cln.h>

using namespace carl;

TEST(NumbersCLN, constructors)
{
    cln::cl_RA b = cln::cl_I(2)/cln::cl_I(3);
    EXPECT_EQ(cln::denominator(b), 3);
    EXPECT_EQ(cln::numerator(b), 2);
}

TEST(NumbersCLN, operations)
{
	ASSERT_TRUE(carl::floor(cln::rationalize(1.5)) == (cln::cl_I)1);
	ASSERT_TRUE(carl::ceil(cln::rationalize(1.5)) == (cln::cl_I)2);
	
	ASSERT_TRUE(carl::gcd((cln::cl_I)15, (cln::cl_I)20) == (cln::cl_I)5);
}

TEST(NumbersCLN, squareroot)
{
    // CLN
    cln::cl_RA b = cln::cl_I(2)/cln::cl_I(3);
    std::pair<cln::cl_RA, cln::cl_RA> resultB;
    resultB = carl::sqrt(b);
    resultB.first.debug_print();
    resultB.second.debug_print();
}

TEST(NumbersCLN, quotient)
{
	EXPECT_EQ(-1, quotient(cln::cl_I(-6),cln::cl_I(7)));
}