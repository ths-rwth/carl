#include "gtest/gtest.h"

#include <carl/formula/bitvector/BVVariable.h>
#include <carl/formula/SortManager.h>

#include "../Common.h"

TEST(BVVariable, Basics)
{
	carl::SortManager& sm = carl::SortManager::getInstance();
	carl::Sort bvSort = sm.addSort("BitVec", carl::VariableType::VT_UNINTERPRETED);
	sm.makeSortIndexable(bvSort, 1, carl::VariableType::VT_BITVECTOR);
	
	carl::Variable a = carl::freshBitvectorVariable("a");
	carl::Variable b = carl::freshBitvectorVariable("b");
	carl::Sort s = carl::getSort("BitVec", std::vector<std::size_t>({8}));
	carl::BVVariable v1;
	carl::BVVariable v2(a, s);
	carl::BVVariable v3(a, s);
	carl::BVVariable v4(b, s);
	
	EXPECT_EQ(v2.sort(), s);
	EXPECT_EQ(v2, v3);
	EXPECT_TRUE(v3 < v4);
	EXPECT_EQ(v2(), a);
	EXPECT_EQ(v4(), b);
	EXPECT_EQ(getOutput(v2), "a");
	EXPECT_EQ(getOutput(v4), "b");
}
