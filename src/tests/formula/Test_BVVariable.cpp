#include "gtest/gtest.h"

#include <carl/formula/bitvector/BVVariable.h>
#include <carl/formula/SortManager.h>

#include "../Common.h"

TEST(BVVariable, Constructor)
{
	carl::SortManager& sm = carl::SortManager::getInstance();
	carl::Sort bvSort = sm.addSort("BitVec", carl::VariableType::VT_UNINTERPRETED);
	sm.makeSortIndexable(bvSort, 1, carl::VariableType::VT_BITVECTOR);
	
	carl::Variable a = carl::freshBitvectorVariable("a");
	carl::Sort s = carl::getSort("BitVec", std::vector<std::size_t>({8}));
	carl::BVVariable v1;
	carl::BVVariable v2(a, s);
}
