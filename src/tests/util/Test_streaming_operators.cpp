#include <carl/io/streamingOperators.h>
using carl::operator<<;

#include "../Common.h"


TEST(StreamingOperators, forward_list)
{
	std::forward_list<int> l {1,2,3,4,5};
	EXPECT_EQ("[1, 2, 3, 4, 5]", getOutput(l));
}

TEST(StreamingOperators, list)
{
	std::list<int> l {1,2,3,4,5};
	EXPECT_EQ("[5: 1, 2, 3, 4, 5]", getOutput(l));
}

TEST(StreamingOperators, map)
{
	std::map<int,int> m;
	m.emplace(1,2);
	m.emplace(3,4);
	m.emplace(5,6);
	EXPECT_EQ("{1 : 2, 3 : 4, 5 : 6}", getOutput(m));
}

TEST(StreamingOperators, multimap)
{
	std::multimap<int,int> m;
	m.emplace(1,2);
	m.emplace(1,3);
	m.emplace(4,5);
	EXPECT_EQ("{1 : 2, 1 : 3, 4 : 5}", getOutput(m));
}

TEST(StreamingOperators, optional)
{
	std::optional<int> o;
	EXPECT_EQ("empty", getOutput(o));
	o = 1;
	EXPECT_EQ("1", getOutput(o));
}

TEST(StreamingOperators, pair)
{
	std::pair<int,int> p {1,2};
	EXPECT_EQ("(1, 2)", getOutput(p));
}

TEST(StreamingOperators, set)
{
	std::set<int> s {1,2,3,4,5,1,2,3,4,5};
	EXPECT_EQ("{5: 1, 2, 3, 4, 5}", getOutput(s));
}

TEST(StreamingOperators, tuple)
{
	std::tuple<int,int,int> t {1,2,3};
	EXPECT_EQ("(1, 2, 3)", getOutput(t));
}

TEST(StreamingOperators, unordered_map)
{
	std::unordered_map<int,int> m;
	m.emplace(1,2);
	m.emplace(3,4);
	m.emplace(5,6);
	EXPECT_EQ("{5 : 6, 1 : 2, 3 : 4}", getOutput(m));
}

TEST(StreamingOperators, unordered_set)
{
	std::unordered_set<int> s {1,2,3,4,5,1,2,3,4,5};
	EXPECT_EQ("{5: 5, 4, 3, 2, 1}", getOutput(s));
}

TEST(StreamingOperators, variant)
{
	std::variant<int,double,std::string> v {1};
	EXPECT_EQ("1", getOutput(v));
	v = 1.5;
	EXPECT_EQ("1.5", getOutput(v));
	v = "foo";
	EXPECT_EQ("foo", getOutput(v));
}

TEST(StreamingOperators, vector)
{
	std::vector<int> v {1,2,3,4,5};
	EXPECT_EQ("[5: 1, 2, 3, 4, 5]", getOutput(v));
}