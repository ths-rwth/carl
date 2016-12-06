#include "gtest/gtest.h"

#include <carl/formula/SortManager.h>
#include <carl/formula/uninterpreted/UFManager.h>
#include <carl/formula/uninterpreted/UninterpretedFunction.h>

#include "../Common.h"

TEST(UninterpretedFunction, Basics)
{
	carl::UninterpretedFunction uf;
	EXPECT_EQ(0, uf.id());
	
	carl::UninterpretedFunction uf2(uf);
	EXPECT_EQ(uf, uf2);
	EXPECT_FALSE(uf < uf2);
	EXPECT_FALSE(uf2 < uf);
	std::hash<carl::UninterpretedFunction> h;
	EXPECT_EQ(h(uf), h(uf2));
}

TEST(UninterpretedFunction, Manager)
{
	carl::SortManager& sm = carl::SortManager::getInstance();
	sm.clear();
	carl::Sort sort = sm.addSort("S", carl::VariableType::VT_UNINTERPRETED);
	
	auto uf = carl::newUninterpretedFunction("f", {}, sort);
	
	EXPECT_EQ("f", uf.name());
	EXPECT_TRUE(uf.domain().empty());
	EXPECT_EQ(sort, uf.codomain());
	EXPECT_EQ("f() S", getOutput(uf));
	
	auto uf2 = carl::newUninterpretedFunction("f", {}, sort);
	EXPECT_EQ(uf, uf2);
}
