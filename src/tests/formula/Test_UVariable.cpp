#include "gtest/gtest.h"

#include <carl/formula/uninterpreted/UVariable.h>
#include <carl/formula/SortManager.h>

#include "../Common.h"

TEST(UVariable, Basics)
{
	carl::SortManager& sm = carl::SortManager::getInstance();
	sm.clear();
	carl::Sort sort = sm.addSort("S", carl::VariableType::VT_UNINTERPRETED);

	carl::Variable a = carl::freshUninterpretedVariable("a");
	carl::Variable b = carl::freshUninterpretedVariable("b");
	carl::UVariable v1;
	carl::UVariable v2(a, sort);
	carl::UVariable v3(a, sort);
	carl::UVariable v4(b, sort);

	EXPECT_EQ(v2.domain(), sort);
	EXPECT_EQ(v2, v3);
	EXPECT_TRUE(v3 < v4);
	EXPECT_EQ(v2.variable(), a);
	EXPECT_EQ(v4.variable(), b);
	EXPECT_EQ(getOutput(v2), "a");
	EXPECT_EQ(getOutput(v4), "b");
	EXPECT_EQ(std::hash<carl::UVariable>()(v2), std::hash<carl::UVariable>()(v3));
}
