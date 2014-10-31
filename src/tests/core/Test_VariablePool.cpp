#include "gtest/gtest.h"

#include "carl/core/VariablePool.h"

using namespace carl;

TEST(VariablePool, singleton)
{
	VariablePool& pool1 = VariablePool::getInstance();
	VariablePool& pool2 = VariablePool::getInstance();
	Variable x = pool1.getFreshVariable();
	std::string name = "test";
	ASSERT_EQ(&pool1, &pool2);

	pool1.setName(x, name);
	ASSERT_EQ(name, pool2.getName(x));
}

TEST(VariablePool, variablePool)
{
	VariablePool& vpool = VariablePool::getInstance();
	Variable x = vpool.getFreshVariable( carl::VariableType::VT_INT );
	vpool.setName(x, "test");
	std::string name = vpool.getName(x, true);
	std::stringstream o;
	o << x;
	ASSERT_EQ(name, o.str());
	
}

TEST(VariablePool, order)
{
    VariablePool& pool = VariablePool::getInstance();
    Variable x = pool.getFreshVariable("x");
    Variable y = pool.getFreshVariable("y");
    
    EXPECT_TRUE(x < y);
    EXPECT_TRUE(y > x);
    EXPECT_FALSE(x > y);
    EXPECT_FALSE(y < x);
}
