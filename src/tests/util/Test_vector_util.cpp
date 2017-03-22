#include "gtest/gtest.h"

#include <carl/util/vector_util.h>

#include <vector>

TEST(VectorUtil, concat)
{
	std::vector<std::vector<std::size_t>> v;
	v.emplace_back(std::initializer_list<std::size_t>({1,2,3}));
	v.emplace_back(std::initializer_list<std::size_t>({4,5}));
	v.emplace_back(std::initializer_list<std::size_t>({6}));
	v.emplace_back(std::initializer_list<std::size_t>({}));

	std::vector<std::size_t> res1;
	carl::vector_concat(res1, v);
	std::vector<std::size_t> res2 = carl::vector_concat(v);
	EXPECT_EQ(res1, res2);
	for (std::size_t i = 1; i < 7; i++) {
		EXPECT_EQ(i, res1[i-1]);
	}
}

TEST(VectorUtil, vector_zip)
{
	std::vector<int> v1({1,2,3});
	std::vector<int> v2({4,5,6});
	std::vector<std::pair<int,int>> res({{1,4},{2,5},{3,6}});
	EXPECT_EQ(res, carl::vector_zip(v1, v2));
}

TEST(VectorUtil, vector_zip_tuple)
{
	std::vector<int> v1({1,2,3});
	std::vector<int> v2({4,5,6});
	std::vector<int> v3({7,8,9});
	std::vector<std::tuple<int,int,int>> res({{1,4,7},{2,5,8},{3,6,9}});
	std::vector<std::tuple<int,int,int>> res2;
	carl::vector_zip_tuple(res2, v1, v2, v3);
	EXPECT_EQ(res, res2);
}
