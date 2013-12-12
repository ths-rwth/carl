#include <gmpxx.h>
#include "gtest/gtest.h"

#include "carl/numbers/typetraits.h"
#include "carl/numbers/operations.h"

#include <cln/cln.h>

using namespace carl;

TEST(Numbers, operations)
{
	ASSERT_TRUE(carl::floor(cln::rationalize(1.5)) == (cln::cl_I)1);
	ASSERT_TRUE(carl::ceil(cln::rationalize(1.5)) == (cln::cl_I)2);
	
	ASSERT_TRUE(carl::gcd((cln::cl_I)15, (cln::cl_I)20) == (cln::cl_I)5);
}
