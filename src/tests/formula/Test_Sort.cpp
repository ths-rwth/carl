#include "gtest/gtest.h"

#include <carl/formula/Sort.h>
#include <carl/formula/SortManager.h>

#include "../Common.h"

TEST(Sort, DefaultConstruction)
{
	::testing::FLAGS_gtest_death_test_style = "threadsafe";

	carl::Sort s;
	EXPECT_TRUE(s.id() == 0);
	EXPECT_TRUE(std::hash<carl::Sort>()(s) == 0);
	EXPECT_DEATH(s.arity(), ".*");
	EXPECT_DEATH(std::cout << s, ".*");
	
	carl::Sort s2;
	EXPECT_TRUE(s == s2);
	EXPECT_FALSE(s < s2);
}

TEST(Sort, SortManager)
{
	carl::SortManager& sm = carl::SortManager::getInstance();
	carl::Sort s = sm.addSort("S");
	carl::Sort s2 = carl::getSort("S");
	EXPECT_TRUE(s == s2);
}
