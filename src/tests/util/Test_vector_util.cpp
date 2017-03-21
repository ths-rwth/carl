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
