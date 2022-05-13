#include <gtest/gtest.h>

#include <carl/core/VariablePool.h>

TEST(VariablePool, singleton)
{
	auto& pool1 = carl::VariablePool::getInstance();
	auto& pool2 = carl::VariablePool::getInstance();
	EXPECT_EQ(&pool1, &pool2);

	carl::Variable x = carl::fresh_real_variable();
	std::string name = "test";
	pool1.set_name(x, name);
	EXPECT_EQ(name, pool2.get_name(x));
}

TEST(VariablePool, variableName)
{
	auto& vpool = carl::VariablePool::getInstance();
	carl::Variable x = carl::fresh_integer_variable("test");
	std::string name = vpool.get_name(x, true);
	std::stringstream o;
	o << x;
	EXPECT_EQ(name, o.str());
}

TEST(VariablePool, order)
{
	carl::Variable x = carl::fresh_real_variable("x");
	carl::Variable y = carl::fresh_real_variable("y");

	EXPECT_TRUE(x < y);
	EXPECT_TRUE(y > x);
	EXPECT_FALSE(x > y);
	EXPECT_FALSE(y < x);
}
