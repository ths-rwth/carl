#include "../Common.h"

#include <carl/util/Timer.h>

#include <chrono>
#include <thread>

TEST(Timer, Basics)
{
	carl::Timer t;
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	EXPECT_TRUE(t.passed() >= 50);
	t.reset();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	EXPECT_TRUE(t.passed() >= 50);
}
