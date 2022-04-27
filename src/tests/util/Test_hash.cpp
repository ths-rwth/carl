#include <carl-common/datastructures/hash.h>
#include <gtest/gtest.h>

TEST(Hash, hash_add)
{
	std::size_t seed = 0;
	std::size_t val = 5;
	std::string s1 = "foo";
	std::string s2 = "foo";
	std::string s3 = "foo";
	std::string s4 = "foo";
	
	
	carl::hash_add(seed, val);
	carl::hash_add(seed, s1);
	carl::hash_add(seed, s2, s3, s4);
}
