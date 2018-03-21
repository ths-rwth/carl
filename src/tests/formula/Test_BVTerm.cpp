#include "gtest/gtest.h"

#include <carl/formula/bitvector/BVTerm.h>
#include <carl/formula/bitvector/BVTermPool.h>

#include "../Common.h"

TEST(BVTerm, Empty)
{
	carl::BVTerm bvt;
}

TEST(BVTerm, Constant)
{
	carl::BVValue val(8, 0);
	EXPECT_EQ(8, val.width());
	carl::BVTerm bvt(carl::BVTermType::CONSTANT, val);
	EXPECT_EQ(8, bvt.width());
	EXPECT_EQ(carl::BVTermType::CONSTANT, bvt.type());
	EXPECT_TRUE(bvt.isConstant());
	EXPECT_EQ(1, bvt.complexity());
	EXPECT_FALSE(bvt.isInvalid());
	EXPECT_EQ(val, bvt.value());
}

TEST(BVTerm, Variable)
{
	carl::SortManager& sm = carl::SortManager::getInstance();
	sm.clear();
	carl::Sort bvSort = sm.addSort("BitVec", carl::VariableType::VT_UNINTERPRETED);
	sm.makeSortIndexable(bvSort, 1, carl::VariableType::VT_BITVECTOR);
	bvSort = carl::getSort("BitVec", std::vector<std::size_t>({8}));
	carl::Variable var = carl::freshBitvectorVariable("a");
	carl::BVVariable bvv(var, bvSort);
	carl::BVTerm bvt(carl::BVTermType::VARIABLE, bvv);
	EXPECT_EQ(8, bvt.width());
	EXPECT_EQ(carl::BVTermType::VARIABLE, bvt.type());
	EXPECT_FALSE(bvt.isConstant());
	EXPECT_EQ(8, bvt.complexity());
	EXPECT_FALSE(bvt.isInvalid());
	EXPECT_EQ(bvv, bvt.variable());
}
