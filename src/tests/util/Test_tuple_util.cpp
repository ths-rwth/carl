#include "../Common.h"

#include <carl/util/tuple_util.h>

int foo(int x, int y) {
	return x + y;
}

TEST(TupleUtil, apply)
{
	std::tuple<int,int> t {1,2};
	EXPECT_EQ(3, carl::tuple_apply(foo, t));
}

TEST(TupleUtil, tail)
{
	std::tuple<int,int,int> t {1,2,3};
	std::tuple<int,int> tail {2,3};
	EXPECT_EQ(tail, carl::tuple_tail(t));
}
