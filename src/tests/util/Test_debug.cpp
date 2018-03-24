#include "../Common.h"

#include <carl/util/debug.h>

TEST(Debug, callingFunction)
{
	std::cout << carl::callingFunction() << std::endl;
}
