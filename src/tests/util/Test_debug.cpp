#include <carl/util/debug.h>
#include <gtest/gtest.h>

TEST(Debug, callingFunction)
{
	std::cout << carl::callingFunction() << std::endl;
}
