#include <carl/formula/Sort.h>
#include <carl/formula/SortManager.h>
#include <carl/formula/model/uninterpreted/SortValue.h>
#include <carl/formula/model/uninterpreted/SortValueManager.h>
#include <gtest/gtest.h>

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
	sm.clear();
	carl::Sort s = sm.addSort("S");
	carl::Sort s2 = carl::getSort("S");
	EXPECT_EQ(s, s2);
}

TEST(Sort, SortValue)
{
	carl::Sort s;
	carl::SortValue sv;
	carl::SortValue sv2(sv);
	EXPECT_EQ(sv.sort(), s);
	EXPECT_EQ(sv.id(), 0);
	EXPECT_EQ(sv, sv2);
}

TEST(Sort, SortValueManager)
{
	carl::SortValueManager& svm = carl::SortValueManager::getInstance();
	carl::Sort s = carl::getSort("S");

	carl::SortValue sv = svm.defaultSortValue(s);
	EXPECT_EQ(sv.id(), 0);
	{
		carl::SortValue sv2 = carl::defaultSortValue(s);
		EXPECT_EQ(sv, sv2);
	}
	{
		carl::SortValue sv1 = carl::newSortValue(s);
		carl::SortValue sv2 = carl::newSortValue(s);
		carl::SortValue sv3 = carl::newSortValue(s);
		EXPECT_EQ(sv1.id(), 1);
		EXPECT_EQ(sv1.sort(), s);
		EXPECT_EQ(sv2.id(), 2);
		EXPECT_EQ(sv2.sort(), s);
		EXPECT_EQ(sv3.id(), 3);
		EXPECT_EQ(sv3.sort(), s);
		EXPECT_TRUE(sv < sv1);
		EXPECT_TRUE(sv1 < sv2);
		EXPECT_TRUE(sv2 < sv3);
	}
}
