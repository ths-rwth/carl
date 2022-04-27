#include <gtest/gtest.h>

#include <carl-common/datastructures/Bitset.h>
#include <carl-covering/carl-covering.h>

TEST(Covering, SetCover) {
	using namespace carl::covering;

	SetCover sc;
	sc.set(0, carl::Bitset({0, 2, 3}));
	sc.set(1, 1);
	sc.set(1, 3);

	EXPECT_EQ(sc.set_count(), 2);
	EXPECT_EQ(sc.get_set(0), carl::Bitset({0, 2, 3}));
	EXPECT_EQ(sc.get_set(1), carl::Bitset({1, 3}));
	EXPECT_EQ(sc.element_count(), 4);
	EXPECT_EQ(sc.largest_set(), 0);
	EXPECT_EQ(sc.get_uncovered(), carl::Bitset({0, 1, 2, 3}));

	sc.select_set(0);

	EXPECT_EQ(sc.set_count(), 2);
	EXPECT_EQ(sc.get_set(0), carl::Bitset());
	EXPECT_EQ(sc.get_set(1), carl::Bitset({1}));
	EXPECT_EQ(sc.element_count(), 4);
	EXPECT_EQ(sc.largest_set(), 1);
	EXPECT_EQ(sc.get_uncovered(), carl::Bitset({1}));

	{
		std::stringstream ss;
		EXPECT_NO_THROW(ss << sc);
	}
}

TEST(Covering, TypedSetCover) {
	using namespace carl::covering;

	TypedSetCover<int> tsc;

	tsc.set(0, carl::Bitset({0, 2, 3}));
	tsc.set(1, 1);
	tsc.set(1, 3);

	const SetCover& sc = tsc.set_cover();

	EXPECT_EQ(sc.set_count(), 2);
	EXPECT_EQ(sc.get_set(0), carl::Bitset({0, 2, 3}));
	EXPECT_EQ(sc.get_set(1), carl::Bitset({1, 3}));
	EXPECT_EQ(sc.element_count(), 4);
	EXPECT_EQ(sc.largest_set(), 0);
	EXPECT_EQ(sc.get_uncovered(), carl::Bitset({0, 1, 2, 3}));
}
