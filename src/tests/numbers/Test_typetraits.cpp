#include "gtest/gtest.h"

#include "../../carl/numbers/numbers.h"

using namespace carl;

TEST(typetraits, is_field)
{
	EXPECT_FALSE(carl::is_field<int>::value);
	EXPECT_TRUE(carl::is_field<cln::cl_RA>::value);
}