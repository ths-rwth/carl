#include "gtest/gtest.h"

#include "carl/util/hash.h"

using namespace carl;

TEST(Hash, hash_combine)
{
	std::size_t seed = 0;
	std::size_t val = 5;
	std::string s1 = "foo";
	std::string s2 = "foo";
	std::string s3 = "foo";
	std::string s4 = "foo";
	
	
	carl::hash_combine(seed, val);
	carl::hash_combine(seed, s1);
	carl::hash_combine(seed, s2, s3, s4);
}
