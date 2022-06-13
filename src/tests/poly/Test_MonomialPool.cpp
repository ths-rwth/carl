#include "gtest/gtest.h"

#include <carl-arith/poly/umvpoly/MonomialPool.h>

using namespace carl;

TEST(MonomialPool, singleton)
{
	MonomialPool& pool1 = MonomialPool::getInstance();
	MonomialPool& pool2 = MonomialPool::getInstance();
	EXPECT_EQ(&pool1, &pool2);
	Variable x = fresh_real_variable("x");
	
	auto m = createMonomial(x, 3);
	EXPECT_EQ(pool2.size(), pool1.size());
}