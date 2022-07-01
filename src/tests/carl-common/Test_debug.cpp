#include <carl-common/debug/debug.h>
#include <gtest/gtest.h>

TEST(Debug, callingFunction)
{
	std::cout << carl::callingFunction() << std::endl;
}
