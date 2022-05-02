#include <carl-common/datastructures/tuple_util.h>
#include <gtest/gtest.h>

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

TEST(TupleUtil, foreach) {
	struct Visitor {
		int operator()(int i) const { return i+1; }
		int operator()(bool i) const { return i ? 1 : 0; }
	};
	EXPECT_EQ(
		std::make_tuple(2,3,0),
		carl::tuple_foreach(Visitor(), std::make_tuple(1,2,false))
	);
}
