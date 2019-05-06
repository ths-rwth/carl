#include <gtest/gtest.h>

#include <carl-covering/carl-covering.h>

using namespace carl::covering;

TypedSetCover<int> get_example() {
	TypedSetCover<int> tsc;
	tsc.set(0, carl::Bitset({1,2}));
	tsc.set(1, carl::Bitset({1,2,3}));
	tsc.set(2, carl::Bitset({0,4}));
	tsc.set(3, carl::Bitset({3,4}));
	tsc.set(4, carl::Bitset({1,2,4,5}));
	return tsc;
}

TEST(heuristics, exact) {
	TypedSetCover<int> tsc = get_example();
	auto cover = tsc.get_cover(heuristic::exact);
	EXPECT_EQ(cover, std::vector<int>({2,3,4}));
}

TEST(heuristics, greedy) {
	TypedSetCover<int> tsc = get_example();
	auto cover = tsc.get_cover(heuristic::greedy);
	EXPECT_EQ(cover, std::vector<int>({1,2,4}));
}

TEST(heuristics, remove_duplicates) {
	TypedSetCover<int> tsc = get_example();
	auto cover = tsc.get_cover(heuristic::remove_duplicates);
	EXPECT_EQ(cover, std::vector<int>());

	const SetCover& sc = tsc;
	// Take care, the columns are reordered!
	EXPECT_EQ(sc.set_count(), 5);
	EXPECT_EQ(sc.get_set(0), carl::Bitset({3}));
	EXPECT_EQ(sc.get_set(1), carl::Bitset({1,3}));
	EXPECT_EQ(sc.get_set(2), carl::Bitset({0,4}));
	EXPECT_EQ(sc.get_set(3), carl::Bitset({1,4}));
	EXPECT_EQ(sc.get_set(4), carl::Bitset({2,3,4}));
	EXPECT_EQ(sc.element_count(), 5);
	EXPECT_EQ(sc.largest_set(), 4);
	EXPECT_EQ(sc.get_uncovered(), carl::Bitset({0, 1, 2, 3, 4}));
}

TEST(heuristics, select_essential) {
	TypedSetCover<int> tsc = get_example();
	auto cover = tsc.get_cover(heuristic::select_essential);
	EXPECT_EQ(cover, std::vector<int>({2, 4}));

	const SetCover& sc = tsc;
	// Take care, the columns are reordered!
	EXPECT_EQ(sc.set_count(), 5);
	EXPECT_EQ(sc.get_set(0), carl::Bitset());
	EXPECT_EQ(sc.get_set(1), carl::Bitset({3}));
	EXPECT_EQ(sc.get_set(2), carl::Bitset());
	EXPECT_EQ(sc.get_set(3), carl::Bitset({3}));
	EXPECT_EQ(sc.get_set(4), carl::Bitset());
	EXPECT_EQ(sc.element_count(), 6);
	EXPECT_EQ(sc.largest_set(), 1);
	EXPECT_EQ(sc.get_uncovered(), carl::Bitset({3}));
}

TEST(heuristics, trivial) {
	TypedSetCover<int> tsc = get_example();
	auto cover = tsc.get_cover(heuristic::trivial);
	EXPECT_EQ(cover, std::vector<int>({0,1,2,3,4}));
}