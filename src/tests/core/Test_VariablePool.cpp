#include "gtest/gtest.h"

#include "carl/core/VariablePool.h"

using namespace carl;

TEST(VariablePool, singleton)
{
	VariablePool& pool1 = VariablePool::getInstance();
	VariablePool& pool2 = VariablePool::getInstance();
	EXPECT_EQ(&pool1, &pool2);

	Variable x = freshRealVariable();
	std::string name = "test";
	pool1.setName(x, name);
	EXPECT_EQ(name, pool2.getName(x));
}

TEST(VariablePool, variableName)
{
	VariablePool& vpool = VariablePool::getInstance();
	Variable x = freshIntegerVariable("test");
	std::string name = vpool.getName(x, true);
	std::stringstream o;
	o << x;
	EXPECT_EQ(name, o.str());
}

TEST(VariablePool, order)
{
    Variable x = freshRealVariable("x");
    Variable y = freshRealVariable("y");
    
    EXPECT_TRUE(x < y);
    EXPECT_TRUE(y > x);
    EXPECT_FALSE(x > y);
    EXPECT_FALSE(y < x);
}
