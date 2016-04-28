#include "gtest/gtest.h"
#include "../../carl/numbers/numbers.h"

TEST(GMP, Debug)
{
	carl::sint i = 4;
	mpq_class n(i);
}
