#include <gtest/gtest.h>

#include <carl/core/Variable.h>
#include <carl/core/VariablePool.h>

#include "../Common.h"

TEST(Variable, Equals)
{
	carl::Variable v1 = carl::freshIntegerVariable("v1");
	carl::Variable v2 = carl::freshRealVariable("v2");
	EXPECT_EQ(carl::VariableType::VT_INT, v1.type());
	EXPECT_EQ(carl::VariableType::VT_REAL, v2.type());
	EXPECT_TRUE(v1 != v2);
	EXPECT_TRUE(v1 == v1);
}

TEST(Variable, Comparison)
{
	ComparisonList<carl::Variable> list;

	carl::Variable v0 = carl::freshRealVariable("v0");
	carl::Variable v1 = carl::freshRealVariable("v1");
	carl::Variable v2 = carl::freshRealVariable("v2");

	list.push_back(v2);
	list.push_back(v1);
	list.push_back(v0);
}
