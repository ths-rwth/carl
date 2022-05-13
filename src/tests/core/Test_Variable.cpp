#include <gtest/gtest.h>

#include <carl/core/Variable.h>
#include <carl/core/VariablePool.h>

#include "../util.h"

TEST(Variable, Equals)
{
	carl::Variable v1 = carl::fresh_integer_variable("v1");
	carl::Variable v2 = carl::fresh_real_variable("v2");
	EXPECT_EQ(carl::VariableType::VT_INT, v1.type());
	EXPECT_EQ(carl::VariableType::VT_REAL, v2.type());
	EXPECT_TRUE(v1 != v2);
	EXPECT_TRUE(v1 == v1);
}

TEST(Variable, Comparison)
{
	ComparisonList<carl::Variable> list;

	carl::Variable v0 = carl::fresh_real_variable("v0");
	carl::Variable v1 = carl::fresh_real_variable("v1");
	carl::Variable v2 = carl::fresh_real_variable("v2");

	list.push_back(v2);
	list.push_back(v1);
	list.push_back(v0);
}
