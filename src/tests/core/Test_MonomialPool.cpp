#include "gtest/gtest.h"

#include "carl/core/MonomialPool.h"

using namespace carl;

TEST(MonomialPool, singleton)
{
	MonomialPool& pool1 = MonomialPool::getInstance();
	MonomialPool& pool2 = MonomialPool::getInstance();
	EXPECT_EQ(&pool1, &pool2);
	Variable x = freshRealVariable("x");
	
	auto m = createMonomial(x, 3);
	EXPECT_EQ(pool2.size(), pool1.size());
}

TEST(MonomialPool, clear)
{
	MonomialPool& pool = MonomialPool::getInstance();
	Variable x = freshRealVariable("x");
	{
		auto m = createMonomial(x, 3);
		EXPECT_GT(pool.size(), 0);
	}

	pool.clear();
	EXPECT_EQ(pool.size(), 0);
	auto m = createMonomial(x, 3);
	EXPECT_EQ(pool.size(), 1);
}
