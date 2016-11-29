#include "gtest/gtest.h"

#include <carl/util/vector_util.h>

#include <vector>

TEST(VectorUtil, concat)
{
	std::vector<std::vector<std::size_t>> v;
	v.push_back({1,2,3});
	v.push_back({4,5});
	v.push_back({6});
	v.push_back({});

	std::vector<std::size_t> res1;
	carl::vector_concat(res1, v);
	std::vector<std::size_t> res2 = carl::vector_concat(v);
	EXPECT_EQ(res1, res2);
	for (std::size_t i = 1; i < 7; i++) {
		EXPECT_EQ(i, res1[i-1]);
	}
}
