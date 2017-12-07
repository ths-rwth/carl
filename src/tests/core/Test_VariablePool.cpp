#include <gtest/gtest.h>

#include <carl/core/VariablePool.h>

TEST(VariablePool, singleton)
{
	auto& pool1 = carl::VariablePool::getInstance();
	auto& pool2 = carl::VariablePool::getInstance();
	EXPECT_EQ(&pool1, &pool2);

	carl::Variable x = carl::freshRealVariable();
	std::string name = "test";
	pool1.setName(x, name);
	EXPECT_EQ(name, pool2.getName(x));
}

TEST(VariablePool, variableName)
{
	auto& vpool = carl::VariablePool::getInstance();
	carl::Variable x = carl::freshIntegerVariable("test");
	std::string name = vpool.getName(x, true);
	std::stringstream o;
	o << x;
	EXPECT_EQ(name, o.str());
}

TEST(VariablePool, order)
{
	carl::Variable x = carl::freshRealVariable("x");
	carl::Variable y = carl::freshRealVariable("y");

	EXPECT_TRUE(x < y);
	EXPECT_TRUE(y > x);
	EXPECT_FALSE(x > y);
	EXPECT_FALSE(y < x);
}
